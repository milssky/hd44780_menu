#include "stm32F10x.h"
#include "hd44780_driver\hd44780_driver.h"
#include "stm32f10x_rcc.h"

#define BUT_UP GPIO_Pin_5
#define BUT_DOWN GPIO_Pin_1
#define BUT_ENTER GPIO_Pin_9
#define BUT_BACK GPIO_Pin_3
#define BUT_PORT GPIOB

#define BUT_OK GPIO_Pin_0 // PA0 for debuffing

void GPIO_Set(void);
void menu(void);
int set_power(void);
int set_impulses(void);
int check_button(int button);
void delay_ms(int msec);
char* itoa(int n);

char * utoa_divmod(int value, char *buffer);

char* percents[] = {"0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"};
int current_menu_position = 0; // текущее место в меню
int need_update = 1; // флаг отрисовки меню
char* menu_string[] = {"Power","Steps"};
int menu_size = 1; //0+1 = 2 пункта
char* qbuf;
int in_menu = 1;
int power_percents = 0;
int impulses_count = 0;

int main(void)
{
  //uint8_t user_char[8]; //Сюда будем записывать пользовательский символ
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE); //Вкл порт С и B
  GPIO_Set();
  lcd_init(); //Инициализируем дисплей
  lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, BLINK); //Включаем курсор и мигалку
  //set_power();
  while(1)
    {

	 menu();

    }

}


void GPIO_Set(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// настройка кнопки вниз и вверх
	GPIO_InitStructure.GPIO_Pin = BUT_DOWN | BUT_UP | BUT_BACK | BUT_OK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BUT_OK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void menu(void)
{
	if((need_update == 1) && (in_menu == 1))
	{
		lcd_clear();
		lcd_out(menu_string[current_menu_position]);
		need_update = 0;
	}
	if(check_button(BUT_DOWN) && (in_menu == 1))
	{
		current_menu_position++;
		if(current_menu_position > menu_size)
		{
			current_menu_position = 0;
		}
		need_update = 1;
	}

	if(check_button(BUT_UP) && (in_menu == 1))
		{
			current_menu_position--;
			if(current_menu_position < 0)
			{
				current_menu_position = 1;
			}
			need_update = 1;
		}

	if(!GPIO_ReadInputDataBit(GPIOB, BUT_ENTER) && (in_menu == 1))
		{
			lcd_clear();
			switch(current_menu_position)
			{
			case 0: set_power();
			case 1: set_impulses();
			}

		}
	/*if(!GPIO_ReadInputDataBit(GPIOB, BUT_BACK) && (in_menu == 1))
	{
		need_update = 1;
		current_menu_position = 0;
	}*/

}

int check_button(int button)
{
	/*
	 * Логика следующая: если кнопка button нажата,
	 * то ждем 10 мс и снова проверяем ее на нажатие, если статус ее не изменился,
	 * то выставляем флаг 1, показывающий, что кнопка нажата
	 */
	if(!GPIO_ReadInputDataBit(BUT_PORT ,button))
	{
		delay_ms(10);
	} else {
		return 0;
	}
	if(!GPIO_ReadInputDataBit(BUT_PORT, button))
	{
		return 1;
	} else {
		return 0;
	}
}

void delay_ms(int msec)
{
	for( ; msec*1000*8 < 0; msec--); // при 8 МГц хех
}

int set_power(void)
{
	int need_up = 1;
	int i = 0;
	in_menu = 0;
	lcd_clear();
	lcd_out("Percents");
	while(GPIO_ReadInputDataBit(BUT_PORT, BUT_OK))
	{
		if(need_up == 1)
		{
			lcd_set_xy(3,1);
			lcd_out(percents[i]);
			need_up = 0;
		}

		if(GPIO_ReadInputDataBit(GPIOA, BUT_OK))
		{
			i++;
			if(i > 9)
			{
				i = 0;
			}
			need_up = 1;
		}
	}
	return i;
}

int set_impulses(void)
{
	int need_up = 1;
	int i = 0; // impulse counter
	in_menu = 0;
	lcd_clear();
	lcd_out("Impulses");
	while(GPIO_ReadInputDataBit(BUT_PORT, BUT_OK))
	{
		if(need_up == 1)
		{
			lcd_set_xy(3,1);
			lcd_out(itoa(i));
			need_up = 0;
		}

		if(GPIO_ReadInputDataBit(GPIOA, BUT_OK))
		{
			i++;
			if(i > 99)
			{
				i = 0;
			}
			need_up = 1;
		}
	}
	return i;

}

char* itoa( int n )
{
register int r, k;
int flag = 0;
int next = 0;
char* s = "";
   if (n < 0) {
      s[next++] = '-';
      n = -n;
   }
   if (n == 0) {
        s[next++] = 0x30;
   } else {
        k = 10000;
        while (k > 0) {
             r = n / k;
             if (flag || r > 0) {
                s[next++] = 0x30 + r;
                flag = 1;
             }
             n -= r * k;
             k = k / 10;
         }
   }
   s[next] = 0;

   return s;
}
