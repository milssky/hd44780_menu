#include "stm32F10x.h"
#include "hd44780_driver\hd44780_driver.h"
#include "stm32f10x_rcc.h"

#define BUT_UP GPIO_Pin_5
#define BUT_DOWN GPIO_Pin_1
#define BUT_ENTER GPIO_Pin_9
#define BUT_BACK GPIO_Pin_3
#define BUT_START GPIO_Pin_2
#define BUT_PORT GPIOB

#define BUT_OK GPIO_Pin_0 // PA0 for debuffing

void GPIO_Set(void);
void EXTI_Configuration(void);
void NVIC_Configuration(void);
void RCC_Configuration(void);
void SetTimTime(int time);
void menu(void);
int set_power(void);
int set_impulses(void);
int check_button(int button);
void delay_ms(int msec);
void itoa(int n, char* buf);

char * utoa_divmod(int value, char *buffer);

char* percents[] = {"0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"};
int current_menu_position = 0; // текущее место в меню
int need_update = 1; // флаг отрисовки меню
char* menu_string[] = {"Power","Steps"};
int menu_size = 1; //0+1 = 2 пункта
char qbuf[5];
int power_table[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50}; // период в мс прерывани€ таймера
int in_menu = 1;
int power_percents = 0;
int impulses_count = 0;
int curr_times = 0;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

int main(void)
{
  RCC_Configuration();
  GPIO_Set();
  NVIC_Configuration();
  lcd_init(); //»нициализируем дисплей
  lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, BLINK); //¬ключаем курсор и мигалку
  menu();
  EXTI_Configuration(); // вышли из меню и включили прерывани€ от детектора нул€
  SetTimTime(power_table[power_percents]);
  while(1)
    {
	  if(curr_times == impulses_count)
	  {
		  EXTI_DeInit();
	  }




    }

}


void GPIO_Set(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// настройка кнопки вниз и вверх
	GPIO_InitStructure.GPIO_Pin = BUT_DOWN | BUT_UP | BUT_BACK | BUT_OK | BUT_ENTER |BUT_START;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* The out to 1st and 2nd thyristors */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BUT_OK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*The EXTI in. PA4 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
}

void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_TIM15 | RCC_APB2Periph_AFIO, ENABLE);
	/* Get takts to GPIOs, TIM15, AFIO, EXTI */
}

void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/* NVIC Configuration */
void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM_Configuration(void)
{


	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 5 - 1; // мощность по умолчанию = 10% (50 мс (период сети) . ћинус стоит, т.к. Prescaler учитываес€ как Prescaler + 1
	TIM_TimeBaseStructure.TIM_Prescaler = 8000; // прерывание раз в миллисекунду: 8ћ√ц / 8000 = 1 к√ц = 1 мс
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_ARRPreloadConfig(TIM15, ENABLE);
	/* Overflow interrupt  */
	TIM_ITConfig(TIM15,TIM_IT_Update,ENABLE);

}

void SetTimTime(int time)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = time;
	TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);
}

void EXTI4_IRQHandler(void)
{
	TIM_Cmd(TIM15, ENABLE);
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
}

void TIM15_IRQHandler(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
	curr_times++;
	TIM_Cmd(TIM15, DISABLE);
}


void menu(void)
{
	while(check_button(BUT_START))
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
}

int check_button(int button)
{
	/*
	 * Ћогика следующа€: если кнопка button нажата,
	 * то ждем 10 мс и снова провер€ем ее на нажатие, если статус ее не изменилс€,
	 * то выставл€ем флаг 1, показывающий, что кнопка нажата
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
	for( ; msec*1000*8 < 0; msec--); // при 8 ћ√ц хех
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
	power_percents = i;
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
			itoa(i,qbuf);
			lcd_out(qbuf);
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
	impulses_count = i;
	return i;

}

void itoa(int n, char* buf)
{
register int r, k;
int flag = 0;
int next = 0;
   if (n < 0) {
      buf[next++] = '-';
      n = -n;
   }
   if (n == 0) {
        buf[next++] = 0x30;
   } else {
        k = 10000;
        while (k > 0) {
             r = n / k;
             if (flag || r > 0) {
                buf[next++] = 0x30 + r;
                flag = 1;
             }
             n -= r * k;
             k = k / 10;
         }
   }
   buf[next] = 0;
}
