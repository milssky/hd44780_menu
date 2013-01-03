#include "hd44780_driver.h"

int RussianFont(uint8_t RusWord){
	switch (RusWord){
		case 0xC0:		//А
			RusWord=0x41;
			break;
		case 0xC1:		//Б
			RusWord=0xA0;
			break;
		case 0xC2:		//В
			RusWord=0x42;
			break;
		case 0xC3:		//Г
			RusWord=0xA1;
			break;
		case 0xC4:		//Д
			RusWord=0xE0;
			break;
		case 0xC5:		//E
			RusWord=0x45;
			break;
		case 0xA8:		//Ё
			RusWord=0xA2;
			break;
		case 0xC6:		//Ж
			RusWord=0xA3;
			break;
		case 0xC7:		//3
			RusWord=0xA4;
			break;
		case 0xC8:		//И
			RusWord=0xA5;
			break;
		case 0xC9:		//Й
			RusWord=0xA6;
			break;
		case 0xCA:		//К
			RusWord=0x4B;
			break;
		case 0xCB:		//Л
			RusWord=0xA7;
			break;
		case 0xCC:		//М
			RusWord=0x4D;
			break;
		case 0xCD:		//H
			RusWord=0x48;
			break;
		case 0xCE:		//O
			RusWord=0x4F;
			break;
		case 0xCF:		//П
			RusWord=0xA8;
			break;
		case 0xD0:		//P
			RusWord=0x50;
			break;
		case 0xD1:		//C
			RusWord=0x43;
			break;
		case 0xD2:		//Т
			RusWord=0x54;
			break;
		case 0xD3:		//У
			RusWord=0xA9;
			break;
		case 0xD4:		//Ф
			RusWord=0xAA;
			break;
		case 0xD5:		//X
			RusWord=0x58;
			break;
		case 0xD6:		//Ц
			RusWord=0xE1;
			break;
		case 0xD7:		//Ч
			RusWord=0xAB;
			break;
		case 0xD8:		//Ш
			RusWord=0xAC;
			break;
		case 0xD9:		//Щ
			RusWord=0xE2;
			break;
		case 0xDA:		//Ъ
			RusWord=0xAD;
			break;
		case 0xDB:		//Ы
			RusWord=0xAE;
			break;
		case 0xDC:		//Ь
			RusWord=0x62;
			break;
		case 0xDD:		//Э
			RusWord=0xAF;
			break;
		case 0xDE:		//Ю
			RusWord=0xB0;
			break;
		case 0xDF:		//Я
			RusWord=0xB1;
			break;

		case 0xE0:		//а
			RusWord=0x61;
			break;
		case 0xE1:		//б
			RusWord=0xB2;
			break;
		case 0xE2:		//в
			RusWord=0xB3;
			break;
		case 0xE3:		//г
			RusWord=0xB4;
			break;
		case 0xE4:		//д
			RusWord=0xE3;
			break;
		case 0xE5:		//е
			RusWord=0x65;
			break;
		case 0xB8:		//ё
			RusWord=0xA2;
			break;
		case 0xE6:		//ж
			RusWord=0xB6;
			break;
		case 0xE7:		//з
			RusWord=0xB7;
			break;
		case 0xE8:		//и
			RusWord=0xB8;
			break;
		case 0xE9:		//й
			RusWord=0xB9;
			break;
		case 0xEA:		//к
			RusWord=0xBA;
			break;
		case 0xEB:		//л
			RusWord=0xBB;
			break;
		case 0xEC:		//м
			RusWord=0xBC;
			break;
		case 0xED:		//н
			RusWord=0xBD;
			break;
		case 0xEE:		//o
			RusWord=0x6F;
			break;
		case 0xEF:		//п
			RusWord=0xBE;
			break;
		case 0xF0:		//р
			RusWord=0x70;
			break;
		case 0xf1:		//c
			RusWord=0x63;
			break;
		case 0xf2:		//т
			RusWord=0xBF;
			break;
		case 0xf3:		//у
			RusWord=0x79;
			break;
		case 0xf4:		//Ф
			RusWord=0xE4;
			break;
		case 0xf5:		//x
			RusWord=0x78;
			break;
		case 0xf6:		//ц
			RusWord=0xE5;
			break;
		case 0xf7:		//ч
			RusWord=0xC0;
			break;
		case 0xf8:		//ш
			RusWord=0xC1;
			break;
		case 0xf9:		//щ
			RusWord=0xE6;
			break;
		case 0xfa:		//ъ
			RusWord=0xC2;
			break;
		case 0xfb:		//ы
			RusWord=0xC3;
			break;
		case 0xfc:		//ь
			RusWord=0xC4;
			break;
		case 0xfd:		//э
			RusWord=0xC5;
			break;
		case 0xfe:		//ю
			RusWord=0xC6;
			break;
		case 0xff:		//я
			RusWord=0xC7;
			break;
	}
	return RusWord;

}


//задержка
void lcd_delay(void) {
	volatile uint32_t tmpvar;
	for (tmpvar=10000;tmpvar!=0;tmpvar--);	//4000
}




void lcd_init() {
	LCD_PORT->CRH |= LCD_PORT_CRH_S;
	LCD_PORT->CRL |= LCD_PORT_CRL_S;
	LCD_PORT->CRH &= ~(LCD_PORT_CRH_C);
	LCD_PORT->CRL &= ~(LCD_PORT_CRL_C);
	lcd_set_4bit_mode();
	lcd_set_state(LCD_ENABLE,CURSOR_ENABLE,NO_BLINK);
	lcd_clear();
	lcd_send(0x06,COMMAND);
}

void lcd_set_user_char(uint8_t char_num, uint8_t * char_data) {
	uint8_t i;
	lcd_send(((1<<6) | (char_num * 8) ), COMMAND);
	for (i=0;i<=7;i++) {
		lcd_send(char_data[i],DATA);
	}
	lcd_send((1<<7), COMMAND);
}

void lcd_set_xy(uint8_t x, uint8_t y)  {
	if (y==0) {
		lcd_send( ((1<<7) | (x)),COMMAND);
	} else {
		lcd_send( ((3<<6) | (x)),COMMAND);
	}
}


void lcd_out(char * txt) {
	while(*txt) {
		lcd_send(*txt,DATA);
		txt++;
	}
}

void lcd_clear(void) {
	lcd_send(0x01,COMMAND);
}

void lcd_set_state(lcd_state state, cursor_state cur_state, cursor_mode cur_mode)  {
	if (state==LCD_DISABLE)  {
		lcd_send(0x08,COMMAND);
	} else {
		if (cur_state==CURSOR_DISABLE) {
			if (cur_mode==NO_BLINK)  {
				lcd_send(0x0C,COMMAND);
			} else {
				lcd_send(0x0D,COMMAND);
			}
		} else  {
			if (cur_mode==NO_BLINK)  {
				lcd_send(0x0E,COMMAND);
			} else {
				lcd_send(0x0F,COMMAND);
			}
		}
	}
}

void lcd_set_4bit_mode(void) {
	lcd_delay();
	lcd_delay();
	lcd_delay();

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);
	LCD_PORT->BSRR=(LCD_DB5_BS);

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);
	LCD_PORT->BSRR=(LCD_DB5_BS);

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);
	LCD_PORT->BSRR=(LCD_DB7_BS);

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();

}

void lcd_send(uint8_t byte, dat_or_comm dc)  {

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);

	if (dc) {
		LCD_PORT->BSRR=LCD_CD_BS;
		//русский шрифт, только если ASCII код больше A0 - точно русский
		if(byte>0xA0){
			byte=RussianFont(byte);
		}

	}

	if (byte & 0x10) {
		LCD_PORT->BSRR=LCD_DB4_BS;
	}
	if (byte & 0x20) {
		LCD_PORT->BSRR=LCD_DB5_BS;
	}
	if (byte & 0x40) {
		LCD_PORT->BSRR=LCD_DB6_BS;
	}
	if (byte & 0x80) {
		LCD_PORT->BSRR=LCD_DB7_BS;
	}

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();


	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC );

	if (byte & 0x01) {
		LCD_PORT->BSRR=LCD_DB4_BS;
	}
	if (byte & 0x02) {
		LCD_PORT->BSRR=LCD_DB5_BS;
	}
	if (byte & 0x04) {
		LCD_PORT->BSRR=LCD_DB6_BS;
	}
	if (byte & 0x08) {
		LCD_PORT->BSRR=LCD_DB7_BS;
	}

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();


}
