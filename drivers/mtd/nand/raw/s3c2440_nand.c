/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#include <nand.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <asm/io.h>

//#define S3C2440_NFCONF_EN          (1<<15)
//#define S3C2440_NFCONF_512BYTE     (1<<14)
//#define S3C2440_NFCONF_4STEP       (1<<13)
#define S3C2440_NFCONT_MAINECCLOCK	(1<<5)
#define S3C2440_NFCONF_INITECC     (1<<4)
#define S3C2440_NFCONF_nFCE        (1<<1)
#define S3C2440_NFCONF_TACLS(x)    ((x)<<12)
#define S3C2440_NFCONF_TWRPH0(x)   ((x)<<8)
#define S3C2440_NFCONF_TWRPH1(x)   ((x)<<4)

#define S3C2440_ADDR_NALE 0x08 	// NFCMMD 0x4E000008
#define S3C2440_ADDR_NCLE 0x0C	// NFADDR 0x4E00000C

#ifdef CONFIG_NAND_SPL

/* in the early stage of NAND flash booting, printf() is not available */
#define printf(fmt, args...)

static void nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	int i;
	struct nand_chip *this = mtd->priv;

	for (i = 0; i < len; i++)
		buf[i] = readb(this->IO_ADDR_R);
}
#endif

static void s3c24x0_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	struct s3c24x0_nand *nand = s3c24x0_get_base_nand();

//	debug("hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE) {
		ulong IO_ADDR_W = (ulong)nand;

		if (!(ctrl & NAND_CLE))
			IO_ADDR_W |= S3C2440_ADDR_NCLE;
		if (!(ctrl & NAND_ALE))
			IO_ADDR_W |= S3C2440_ADDR_NALE;

		chip->IO_ADDR_W = (void *)IO_ADDR_W;

		if (ctrl & NAND_NCE)
			writel(readl(&nand->nfconf) & ~S3C2440_NFCONF_nFCE,
			       &nand->nfconf);
		else
			writel(readl(&nand->nfconf) | S3C2440_NFCONF_nFCE,
			       &nand->nfconf);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);
}

static int s3c24x0_dev_ready(struct mtd_info *mtd)
{
	struct s3c24x0_nand *nand = s3c24x0_get_base_nand();
//	debug("dev_ready\n");
	return readl(&nand->nfstat) & 0x01;
}

#ifdef CONFIG_S3C2440_NAND_HWECC
void s3c24x0_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c24x0_nand *nand = s3c24x0_get_base_nand();
	debug("s3c24x0_nand_enable_hwecc(%p, %d)\n", mtd, mode);
	writel(readl(&nand->nfcont) | S3C2440_NFCONF_INITECC, &nand->nfcont);
	writel(readl(&nand->nfcont) & (~S3C2440_NFCONT_MAINECCLOCK), &nand->nfcont);
}

static int s3c24x0_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
				      u_char *ecc_code)
{
	struct s3c24x0_nand *nand = s3c24x0_get_base_nand();
	// ecc_code[0] = readb(&nand->nfmecc0);
	// ecc_code[1] = readb(&nand->nfmecc0 + 1);
	// ecc_code[2] = readb(&nand->nfmecc0 + 2);
	// ecc_code[3] = readb(&nand->nfmecc0 + 3);

	ecc_code[0] = readl(&nand->nfmecc0) & 0xFF;
	ecc_code[1] = (readl(&nand->nfmecc0) >> 8) & 0xFF;
	ecc_code[2] = (readl(&nand->nfmecc0) >> 16) & 0xFF;
	ecc_code[3] = (readl(&nand->nfmecc0) >> 24) & 0xFF;

	debug("%x\n", readl(&nand->nfmecc0));
	debug("s3c24x0_nand_calculate_hwecc(%p,): 0x%02x 0x%02x 0x%02x 0x%02x\n",
	      mtd , ecc_code[0], ecc_code[1], ecc_code[2], ecc_code[3]);

	return 0;
}

static int s3c24x0_nand_correct_data(struct mtd_info *mtd, u_char *dat,
				     u_char *read_ecc, u_char *calc_ecc)
{
	if (read_ecc[0] == calc_ecc[0] &&
	    read_ecc[1] == calc_ecc[1] &&
	    read_ecc[2] == calc_ecc[2] && 
		read_ecc[3] == calc_ecc[3])
		return 0;

	printf("s3c24x0_nand_correct_data: failed\n");
	return -1;
}
#endif

static void s3c2440_nand_select_chip(struct mtd_info *mtd, int chipnr)
{
	struct s3c24x0_nand *nand = s3c24x0_get_base_nand();

	switch (chipnr) {
	case -1:
		writel(readl(&nand->nfconf) | S3C2440_NFCONF_nFCE,
			       &nand->nfconf);
		break;
	case 0:
		writel(readl(&nand->nfcont) & ~S3C2440_NFCONF_nFCE,
			       &nand->nfcont);
		break;

	default:
		BUG();
	}
}

int board_nand_init(struct nand_chip *nand)
{
	u_int32_t cfg;
	u_int8_t tacls, twrph0, twrph1;
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	struct s3c24x0_nand *nand_reg = s3c24x0_get_base_nand();

	debug("board_nand_init()\n");

	/* enable nand flash clock */
	writel(readl(&clk_power->clkcon) | (1 << 4), &clk_power->clkcon);

	/* initialize hardware */
	cfg = S3C2440_NFCONF_TACLS(0) 
		| S3C2440_NFCONF_TWRPH0(1) 
		| S3C2440_NFCONF_TWRPH1(0);
	writel(cfg, &nand_reg->nfconf);

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = (void *)&nand_reg->nfdata;
	nand->IO_ADDR_W = (void *)&nand_reg->nfdata;

	nand->select_chip = s3c2440_nand_select_chip;

	/* read_buf and write_buf are default */
	/* read_byte and write_byte are default */
#ifdef CONFIG_NAND_SPL
	nand->read_buf = nand_read_buf;
#endif

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s3c24x0_hwcontrol;

	nand->dev_ready = s3c24x0_dev_ready;

#ifdef CONFIG_S3C2440_NAND_HWECC
//	writel(readl(&nand_reg->nfcont) | S3C2440_NFCONF_INITECC, &nand_reg->nfcont);
	nand->ecc.hwctl = s3c24x0_nand_enable_hwecc;
	nand->ecc.calculate = s3c24x0_nand_calculate_ecc;
	nand->ecc.correct = s3c24x0_nand_correct_data;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.size = CONFIG_SYS_NAND_ECCSIZE;
	nand->ecc.bytes = CONFIG_SYS_NAND_ECCBYTES;
	nand->ecc.strength = 1;
#else
	nand->ecc.mode = NAND_ECC_SOFT;
#endif

#ifdef CONFIG_S3C2440_NAND_BBT
	nand->bbt_options |= NAND_BBT_USE_FLASH;
#endif

	debug("end of nand_init\n");

	return 0;
}
