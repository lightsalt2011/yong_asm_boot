
#include "./include/s5pc110.h"
#include "./include/s5pc11x.h"
#include "./include/types.h"

char * strcpy(char *dest, const char *src)
{
	int i = 0;
	const char *ptr = src;

//	if ((dest == NULL) || (src == NULL)) 
//		return  NULL;
	
	while(*ptr) {
		dest[i] = ptr[i];
		i++;
		ptr++;
	}
	
	return dest;
}

int strlen(char *str)
{
	int len = 0;

	while(str[len++]); 
	
	return len;	
}

/* uart init ref */
void uart_init(void)
{
	S5PC11X_UART *uart = (S5PC11X_UART *)(ELFIN_UART_CONSOLE_BASE);//uart0
	
	GPA0_CON = 0x2222;//配置GPIO
	
	uart->UFCON = 0x0;//不使用 fifo
	uart->UMCON = 0x0;//不使用 modem
	uart->ULCON = 0x3;//1bit stop; 8bit data
	uart->UCON = 0x3c5;//polling or interrupt mode
	uart->UBRDIV = UART_UBRDIV_VAL;
	uart->UDIVSLOT = UART_UDIVSLOT_VAL;
	uart->UTXH= 'L';
}

static void _putc(char c)
{
	S5PC11X_UART *uart = (S5PC11X_UART *)(ELFIN_UART_CONSOLE_BASE);

	/* wait for room in the tx FIFO */
	while (!(uart->UTRSTAT & 0x2));

	uart->UTXH = c;

	/* If \n, also do \r */
	if (c == '\n')
		_putc('\r');
}

void puts(const char *s)
{
	while (*s) {
		_putc(*s++);
	}
}

/*nand init ref */
void nand_init(void)
{
	/* 配置 选择nand 功能, nand 0 */
	MP01CON_REG &= ~((0xf<<8)); 
	MP01CON_REG |= (0x3<<8); 
	MP01PUD_REG &= ~((0x3<<4)); 
	
	MP03CON_REG &= ~(0xFFFFFF);
	MP03CON_REG = 0x22222222;
	MP03PUD_REG &= ~0x3fff;
	
	NFCONF_REG &=  ~0x777F;
	NFCONF_REG |= NFCONF_VAL;
	
	/*[0]:mode, 0:disable nand flash control; 1:enable nand flash control */
	NFCONT_REG &= ~0x707CF;
	NFCONT_REG |= NFCONT_VAL;
}

/*gpio init reg*/
void led_on_test(void)
{
	GPH0CON_REG &= ~((0x0f<<1*4) | (0x0f<<2*4) | (0x0f<<3*4));
	GPH0CON_REG |= ((0x01<<1*4) | (0x01<<2*4) | (0x01<<3*4));

	GPH0PUD_REG &= ~((0x03<<1*2) | (0x03<<2*2) | (0x03<<3*2));
	GPH0PUD_REG |= ((0x02<<1*2) | (0x02<<1*2) | (0x02<<1*2));

	GPH0DAT_REG &= ~((0x01<<1*1) | (0x01<<1*2) | (0x01<<1*3));
	GPH0DAT_REG |= ( (0x01<<1*1) | (0x01<<1*2) | (0x01<<1*3));
	
}


