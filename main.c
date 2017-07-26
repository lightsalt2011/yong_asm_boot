#include "./include/s5pc110.h"
#include "./include/s5pc11x.h"
#include "./include/nand.h"
#include "./include/smdkv210single.h"
#include "./include/setup.h"
#include "./include/types.h"

extern void puts(const char *s);
extern void led_on_test(void);
extern char * strcpy(char *dest, const char *src);
extern int strlen(char *str);

void ndelay(int count)
{
	int i = 0;
	for (; i<count; i++);
}
//nand_readpage(0, addr, buf, page_size);
static void nand_read(long src, uchar* dest, int len)
{
	int i = 0;
	int row;
	int col = src % 2048;
	
	NAND_ENABLE_CE();

	while(i < len) {
		
		//puts("for test");
		//puts("for test\n");
		
		col = src % 2048;
		row = src / 2048;
		
		NFCMD_REG = NAND_CMD_READ0;
		
		/* Write Column Address */
		NFADDR_REG = (col) & 0xff;
		ndelay(100);
		NFADDR_REG = (col >> 8) & 0xff;
		ndelay(100);

		/* Write Row Address */
		NFADDR_REG = (row) & 0xff;
		ndelay(100);
		NFADDR_REG = (row >> 8) & 0xff;
		ndelay(100);
		NFADDR_REG = (row >> 16) & 0xff;
		ndelay(100);


		NFCMD_REG = NAND_CMD_READSTART;
		ndelay(100);

		NF_TRANSRnB();

		for (; (col<2048) && (i<len); i++) {
			dest[i] = NFDATA8_REG;
			src++;
			col++;
		}
		
		col = 0;
		row = 0;
	}
	NAND_DISABLE_CE();
}

//nand_read((unsigned int)src, dest, len);
void copy_uboot_to_ram(void)
{
	uchar *buf = (uchar *)0x33e00000;
	long boot_size = 0x80000;
	
	//led_on_test();
	
	//puts("aaaaaaaaaaaaa");
	
	nand_read(0, buf, boot_size);
}

void param_set(struct tag *param)
{

	char *ptr = "root=/dev/mtdblock4 rootfstype=yaffs2 init=/linuxrc console=ttySAC0,115200";
		
	param->hdr.tag = ATAG_CORE;
	param->hdr.size = tag_size(tag_core);
	param->u.core.flags = 0;
	param->u.core.pagesize = 0;
	param->u.core.rootdev = 0;
	param = tag_next(param);

	
	param->hdr.tag = ATAG_MEM;
	param->hdr.size = tag_size (tag_mem32);//(struct tag_header)+(struct tag_mem32)
	param->u.mem.start = 0x30000000;
	param->u.mem.size = 0x10000000;
	param = tag_next (param);


	param->hdr.tag = ATAG_CMDLINE;
	param->hdr.size = (sizeof (struct tag_header) + strlen (ptr) + 1 + 4) >> 2;
	strcpy (param->u.cmdline.cmdline, ptr);
	param = tag_next (param);


	param->hdr.tag = ATAG_NONE;
	param->hdr.size = 0;
	
}

void copy_kernel_to_ram(void)
{
	uchar *buf = (uchar *)0x30008000;
	long kernel_size = 0x500000; //5M
	long kernel_base = 0x600000; //
	
	//led_on_test();
	
	puts("kernel is coming");
	
	nand_read(kernel_base, buf, kernel_size);
}


void my_boot(void)
{
	void	(*theKernel)(int zero, int arch, uint params);
	struct	tag *params_start;
	
	params_start = (struct	tag *)(0x30000000+0x100);
	param_set(params_start);
	
	puts("\n kernel is ready to go go go ......\n");

	copy_kernel_to_ram();
	
	theKernel = (void (*)(int, int, uint))0x30008000;
	theKernel (0, 0x998, 0x30000100);
}

int main(void)
{
	puts("hello yong_boot is coming\n");
	puts("boot has copyed to dram\n");

	
	my_boot();
	
	//while(1);

	return 0;	
}



