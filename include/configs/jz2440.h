#ifndef __JZ2440_H
#define __JZ2440_H

#define CONFIG_SYS_LOAD_ADDR 	0x30800000

#define CONFIG_SYS_CLK_FREQ 	12000000

#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)

#define CONFIG_S3C24X0_SERIAL 
#define CONFIG_SERIAL1

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE 	PHYS_SDRAM_1 
#define CONFIG_SYS_INIT_SP_ADDR 	(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
				GENERATED_GBL_DATA_SIZE)

#define PHYS_SDRAM_1 			0x30000000
#define PHYS_SDRAM_1_SIZE 		0x04000000

#endif
