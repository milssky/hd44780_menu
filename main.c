#include "stm32F10x.h"
#include "hd44780_driver\hd44780_driver.h"
#include "stm32f10x_rcc.h"

//#define BUT_OK GPIO_Pin_0 // PA0 for debuffing

#define BUT_START GPIO_Pin_14
#define BUT_UP GPIO_Pin_10
#define BUT_DOWN GPIO_Pin_11
#define BUT_ENTER GPIO_Pin_12
#define BUT_BACK GPIO_Pin_13
#define BUT_PORT GPIOB

#define ADC_PIN GPIO_Pin_1

void GPIO_Configuration(void);
void EXTI_Configuration(void);
void NVIC_Configuration(void);
void RCC_Configuration(void);
void TIM_Configuration(void);
void ADC_Configuration(void);
void SetTimTime(int time);
void menu(void);
void okey(void);
int set_power(void);
int set_impulses(void);
int check_button(int button);
void delay_ms(int msec);
void itoa(int n, char* buf);

char* percents[] = {"0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"};
int current_menu_position = 0;
int need_update = 1;
unsigned char flag=0;
char* menu_string[] = {"Power","Impulses"};
int menu_size = 1;
char qbuf[5];
int power_table[] = {50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0};
int in_menu = 1;
int power_percents = 0;
int impulses_count = 0;
int curr_times = 0;
int adc_current;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

int main(void)
{
  RCC_Configuration();
  GPIO_Configuration();
  NVIC_Configuration();
  TIM_Configuration();
  ADC_Configuration();
  /*GPIO_SetBits(GPIOC, GPIO_Pin_8);
  GPIO_SetBits(GPIOC, GPIO_Pin_9);*/
  lcd_init();
  lcd_set_state(LCD_ENABLE, CURSOR_ENABLE, BLINK);
  menu();
  okey();
  SetTimTime(power_table[power_percents]);
  EXTI_Configuration();
  lcd_clear();
  lcd_out("Current");
  lcd_set_xy(0,1);
  while(1)
    {
	  /*if (curr_times != 5)
	  {
		  adc_current += ADC_GetConversionValue(ADC1);
		  curr_times++;
	  } else {
		  itoa(adc_current,qbuf);
		  lcd_out(qbuf);
		  curr_times = 0;
	  }*/

	  if(flag) // произошел переход через ноль и дождались конца отсчета таймера
	      {
	        GPIO_SetBits(GPIOC, GPIO_Pin_8);
	      }
	      else
	      {
	        GPIO_ResetBits(GPIOC, GPIO_Pin_8);

	      }
	  if (!check_button(BUT_START)) // нажали второй раз на старт и выключили сварку
	  {
		  EXTI_DeInit();
	  }

    }

}
/*
 *
 */
void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA |
							RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	/* Get takts to GPIOs, TIM15, AFIO, EXTI */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

/*
 *
 */
void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	/* Configure EXTI4 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*
 *
 */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = BUT_DOWN | BUT_UP | BUT_BACK | BUT_ENTER |BUT_START;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* The out to 1st and 2nd thyristors */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/*The EXTI in. PA4 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

	/* ADC Pin */

	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*
 *
 */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* Enable and set EXTI4 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 *
 */
void TIM_Configuration(void)
{
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 5;
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	/* Overflow interrupt  */
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

}

/*
 *
 */
void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	//clock for ADC (max 14MHz --> 72/6=12MHz)
	RCC_ADCCLKConfig (RCC_PCLK2_Div6);
	// enable ADC system clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// define ADC config
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	// we work in continuous sampling mode
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;

	ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 1,ADC_SampleTime_28Cycles5); // define regular conversion config
	ADC_Init(ADC1, &ADC_InitStructure);	//set config of ADC1

	// enable ADC
	ADC_Cmd(ADC1,ENABLE);	//enable ADC1

	//	ADC calibration (optional, but recommended at power on)
	ADC_ResetCalibration(ADC1);	// Reset previous calibration
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
	while(ADC_GetCalibrationStatus(ADC1));

	// start conversion
	ADC_Cmd (ADC1,ENABLE);	//enable ADC1
}

/*
 *
 */
void SetTimTime(int time)
{


	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = time;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
}

/*
 *
 */
void EXTI4_IRQHandler(void)
{
	TIM_Cmd(TIM2, ENABLE);
	EXTI_ClearITPendingBit(EXTI_Line4);
}

/*
 *
 */
void TIM2_IRQHandler(void)
{
	flag ^= 1;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// start conversion (will be endless as we are in continuous mode). we started adc conversion while thyristor opened.
	TIM_Cmd(TIM2, DISABLE);
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

}

/*
 *
 */
void menu(void)
{
	while(check_button(BUT_START))
	{
		if((need_update == 1) && (in_menu == 1))
		{
			lcd_clear();
			lcd_out(menu_string[current_menu_position]);
			lcd_set_xy(0,1);
			itoa(power_percents,qbuf); // debuging
			lcd_out(qbuf);
			lcd_set_xy(3,1);
			itoa(impulses_count,qbuf);
			lcd_out(qbuf);
			need_update = 0;
		}
		if(!check_button(BUT_DOWN) && (in_menu == 1))
		{
			current_menu_position++;
			if(current_menu_position > menu_size)
			{
				current_menu_position = 0;
			}
			need_update = 1;
		}

		if(!check_button(BUT_UP) && (in_menu == 1))
			{
				current_menu_position--;
				if(current_menu_position < 0)
				{
					current_menu_position = 1;
				}
				need_update = 1;
			}

		if(!check_button(BUT_ENTER) && (in_menu == 1))
			{
				lcd_clear();
				switch(current_menu_position)
				{
				case 0: set_power();
						break;
				case 1: set_impulses();
						break;
				}
				need_update = 1;

			}
		if(!check_button(BUT_BACK) && (in_menu == 1))
		{
			need_update = 1;
			current_menu_position = 0;
		}
	}
}

void okey(void)
{
	lcd_clear();
	itoa(impulses_count, qbuf);
	lcd_out("Imp:");
	lcd_out(qbuf);
	lcd_set_xy(0,1);
	lcd_out("Perc:");
	itoa(power_table[power_percents],qbuf);
	lcd_out(qbuf);
	while(check_button(BUT_START))
	{

	}
}
int check_button(int button)
{
	/*
	 *
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

/*
 *
 */
void delay_ms(int msec)
{
	for( ; msec*1000*8 > 0; msec--); // ïðè 8 ÌÃö õåõ
}

/*
 *
 */
int set_power(void)
{
	int need_up = 1;
	int i = 0;
	int flag = 0;
	in_menu = 0;
	lcd_clear();
	lcd_out("Percents");
	while(flag == 0)
	{
		if(need_up == 1)
		{
			lcd_set_xy(3,1);
			lcd_out(percents[i]);
			need_up = 0;
		}

		if(!check_button(BUT_UP))
		{
			i++;
			if(i > 9)
			{
				i = 0;
			}
			need_up = 1;
		}

		if(!check_button(BUT_DOWN))
		{
			i--;
			if(i < 0)
			{
				i = 9;
			}
			need_up = 1;
		}

		if(!check_button(BUT_ENTER))
		{
			delay_ms(1000);
			if(!check_button(BUT_ENTER))
			{
				flag = 1;
				lcd_clear();
				delay_ms(1000);
			}
		}

		if(!check_button(BUT_BACK))
		{
			power_percents = 0;
			in_menu = 1;
			return 0;
		}
	}
	power_percents = i;
	in_menu = 1;
	return i;
}

/*
 *
 */
int set_impulses(void)
{
	int need_up = 1;
	int i = 0; // impulse counter
	in_menu = 0;
	int flag = 0;
	lcd_clear();
	lcd_out("Impulses");
	while(flag == 0)
	{
		if(need_up == 1)
		{
			lcd_set_xy(3,1);
			itoa(i,qbuf);
			lcd_out(qbuf);
			need_up = 0;
		}

		if(!check_button(BUT_UP))
		{
			i++;
			if(i > 99)
			{
				i = 0;
			}
			need_up = 1;
		}

		if(!check_button(BUT_DOWN))
		{
			i--;
			if(i < 0)
			{
				i = 99;
			}
			need_up = 1;
		}

		if(!check_button(BUT_ENTER))
		{
			delay_ms(1000);
			if(!check_button(BUT_ENTER))
			{
				flag = 1;
				lcd_clear();
				delay_ms(1000);
			}
		}

		if(!check_button(BUT_BACK))
		{
			impulses_count = 0;
			in_menu = 1;
			return 0;
		}
	}
	in_menu = 1;
	impulses_count = i;
	return i;

}

/*
 *
 */
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
