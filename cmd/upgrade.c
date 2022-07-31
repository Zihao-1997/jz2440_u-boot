#include <common.h>
#include <command.h>
#include <net.h>
#include <nand.h>
#include <jffs2/jffs2.h>
#include "legacy-mtd-utils.h"

int do_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int tftp_size;
    nand_erase_options_t opts;
    loff_t off, size, maxsize;
    struct mtd_info *mtd;
    int dev = nand_curr_device;
 //   char * const str[] = { "u-boot" };
    size_t rwsize;

    if (argc < 2) {
        printf("usage: upgrade [part-name]\n");
        return CMD_RET_FAILURE;
    }

    /* copy binary file to sdram via network using TFTP protocol */
    load_addr = 0x30000000;
    net_boot_file_name_explicit = true;
    if (strcmp(argv[1], "u-boot") == 0)
        copy_filename(net_boot_file_name, "u-boot.bin", sizeof(net_boot_file_name));
    else if (strcmp(argv[1], "kernel") == 0)
        copy_filename(net_boot_file_name, "uImage", sizeof(net_boot_file_name));

    bootstage_mark(BOOTSTAGE_ID_NET_START);

	tftp_size = net_loop(TFTPGET);
	if (tftp_size < 0) {
		bootstage_error(BOOTSTAGE_ID_NET_NETLOOP_OK);
		return CMD_RET_FAILURE;
	}

	bootstage_mark(BOOTSTAGE_ID_NET_NETLOOP_OK);

    mtd = get_nand_dev_by_index(dev);
    if (mtd_arg_off_size(1, argv+1, &dev, &off, &size,
                    &maxsize, MTD_DEV_TYPE_NAND,
                    mtd->size) != 0)
        return CMD_RET_FAILURE;

    /* erase the corresponding part of nandflash */
    memset(&opts, 0, sizeof(opts));
    opts.offset = off;
    opts.length = size;
    opts.jffs2  = 0;
    opts.quiet  = 0;
    opts.spread = 0;

    if (nand_erase_opts(mtd, &opts) != 0) {
        printf("nandflash erase error\n");
        return CMD_RET_FAILURE;
    }

    /* write the corresponding part of nandflash */
    rwsize = size;
    if (nand_write_skip_bad(mtd, off, &rwsize,
							  NULL, maxsize,
							  (u_char *)0x30000000,
							  WITH_WR_VERIFY) != 0) {
        printf("nandflash write error\n");
        return CMD_RET_FAILURE;
    } else {
        printf(" %zu bytes written: OK\n", rwsize);
    }

    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
    upgrade, 2, 1, do_upgrade, 
    "upgrade u-boot/kernel/file-system via network using TFTP protocol.", 
    "[partName]"
);
