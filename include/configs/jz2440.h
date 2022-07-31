#ifndef __JZ2440_H
#define __JZ2440_H

#define CONFIG_SYS_LOAD_ADDR 	0x30800000

#define CONFIG_SYS_CLK_FREQ 	12000000

#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE 	PHYS_SDRAM_1 
#define CONFIG_SYS_INIT_SP_ADDR 	(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
				GENERATED_GBL_DATA_SIZE)

#define PHYS_SDRAM_1 			0x30000000
#define PHYS_SDRAM_1_SIZE 		0x04000000

/* NAND FLASH config */
//#define CONFIG_CMD_NAND
//#define CONFIG_MTD_RAW_NAND
#define CONFIG_SYS_NAND_BASE 	0x4E000000
#define CONFIG_SYS_MAX_NAND_DEVICE 	1
#define CONFIG_SYS_NAND_ECCSIZE 	2048
#define CONFIG_SYS_NAND_ECCBYTES	4

/* */
#define CONFIG_DM9000_BASE 		0x20000000
#define DM9000_IO 				(CONFIG_DM9000_BASE)
#define DM9000_DATA 			(CONFIG_DM9000_BASE + 4)

#define MTDIDS_DEFAULT 			"nand0=jz2440-0"
#define MTDPARTS_DEFAULT 		"mtdparts=jz2440-0:512k(u-boot)," 	\
								"128k(params),4m(kernel),-(rootfs)"

#define CONFIG_ETHADDR			00:0c:29:4d:e4:f4 
#define CONFIG_IPADDR			192.168.2.4
#define CONFIG_SERVERIP 		192.168.2.2
#define CONFIG_NETMASK 			255.255.255.0

#define CONFIG_BOOTCOMMAND 		"nand read 0x30000000 kernel; bootm 0x30000000"
#define CONFIG_BOOTARGS 	"console=ttySAC0 root=/dev/mtdblock3 earlyprintk"

#define CONFIG_SETUP_MEMORY_TAGS 

#endif
