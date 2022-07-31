#define NFCONF  (*((volatile unsigned long *)0x4E000000))
#define NFCONT  (*((volatile unsigned long *)0x4E000004))
#define NFCMMD  (*((volatile unsigned char *)0x4E000008))
#define NFADDR  (*((volatile unsigned char *)0x4E00000C))
#define NFDATA  (*((volatile unsigned char *)0x4E000010))
#define NFSTAT  (*((volatile unsigned char *)0x4E000020))

#define TACLS   0 
#define TWRPH0  1 
#define TWRPH1  0 

#define PAGE_SIZE   2048

void clear_bss(void)
{
    extern int __bss_start, __bss_end;
    int *p = &__bss_start;

    for (; p < &__bss_end; p++)
        *p = 0;
}

void nand_init_rel(void)
{
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
    /**
     * initialize ECC
     * disable chip select 
     * enable NAND flash controller
     **/
    NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
}

static void nand_chip_select(unsigned char enable)
{
    if (enable) {
        NFCONT &= ~(1 << 1);
    }
    else {
        NFCONT |= (1 << 1);
    }
}

static void nand_cmd(unsigned char cmd)
{
    volatile int i;
    NFCMMD = cmd;
    for (i = 0; i < 10; i++);
}

static unsigned char nand_data(void)
{
    return NFDATA; 
}

static void nand_addr(unsigned int addr)
{
    unsigned int col = addr % PAGE_SIZE; 
    unsigned int page = addr / PAGE_SIZE; 
    volatile int i;

    NFADDR = col & 0xFF; 
    for (i = 0; i < 10; i++);
    NFADDR = (col >> 8) & 0xFF; 
    for (i = 0; i < 10; i++);
    NFADDR = page & 0xFF; 
    for (i = 0; i < 10; i++);
    NFADDR = (page >> 8) & 0xFF; 
    for (i = 0; i < 10; i++);
    NFADDR = (page >> 16) & 0xFF; 
    for (i = 0; i < 10; i++);
}

static void nand_wait_ready(void)
{
    while (!(NFSTAT & 0x01));
}

static int nand_is_bad(unsigned int addr)
{
    unsigned int col = 2048;
    unsigned int page = addr / PAGE_SIZE; 
    unsigned char data = 0;
    volatile int i;

    nand_chip_select(1);
    nand_cmd(0x00); 
    NFADDR = col & 0xFF; 
    for (i = 0; i < 10; i++);
    NFADDR = (col >> 8) & 0xFF; 
    for (i = 0; i < 10; i++); 
    NFADDR = page & 0xFF; 
    for (i = 0; i < 10; i++); 
    NFADDR = (page >> 8) & 0xFF; 
    for (i = 0; i < 10; i++); 
    NFADDR = (page >> 16) & 0xFF; 
    for (i = 0; i < 10; i++);
    nand_cmd(0x30); 
    nand_wait_ready(); 
    data = nand_data();
    nand_chip_select(0);
    if (data == 0xFF) 
        return 0; 
    else 
        return 1;
}

void nand_read(unsigned int addr, unsigned char *buf, unsigned int size)
{
    int i = 0; 
    unsigned int col = addr % PAGE_SIZE; 

    nand_chip_select(1); 
    while (i < size) {
        // if (!(addr & 0x1FFFF) && nand_is_bad(addr)) {
        //     addr += (128 * 1024);
        //     continue;
        // }

        nand_cmd(0x00); 
        nand_addr(addr); 
        nand_cmd(0x30); 
        nand_wait_ready(); 
        for (; (col < PAGE_SIZE) & (i < size); i++, col++) {
            buf[i] = nand_data(); 
            addr++; 
        }

        col = 0; 
    }
    nand_chip_select(0);
}

void nand_relocate(unsigned char *src, unsigned char *des, unsigned int size)
{
    nand_read((unsigned int)src, des, size);
}
