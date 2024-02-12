#include"config.h"
#include"stm32f10x.h"
#include"stack.h"


#define SPEED_25HZ 36-1
#define SPEED_2HZ 450-1

void (*blink_led)(void);


void CLK_Config(void)
{
	/*resets the RCC configuration to its default state. */
	RCC_DeInit();
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	/*wait start up HSE*/
	while(!RCC_WaitForHSEStartUp());
	
	/* Wait till HSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)==RESET);
	
	// Enable the Flash prefetch buffer to improve memory access performance
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	// Set the Flash latency to 0 wait states for optimal access speed
	FLASH_SetLatency(FLASH_Latency_2);
	
	/* HCLK = SYSCLK/2 */
	RCC_HCLKConfig(RCC_SYSCLK_Div2);
	 /* PCLK1 = HCLK/4 */
	RCC_PCLK1Config(RCC_HCLK_Div4);
	/* PCLK2 = HCLK/2 */
  RCC_PCLK2Config(RCC_HCLK_Div2);
  /* PLLCLK = 9 * HSE(8 MHz) */
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
	/* Enable PLL */
  RCC_PLLCmd(ENABLE);
  /* Wait till PLL is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  /* Wait till PLL is used as system clock source */
  while(RCC_GetSYSCLKSource() != 0x08);
		
	/* Enable LED1 ,LED0 use APB2 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD,ENABLE);
	
	/*EXTI*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	/* Enable TIM */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef	GPIO_Struct;
	
	// Configure LED pin as output Push-Pull mode
	GPIO_Struct.GPIO_Mode=			GPIO_Mode_Out_PP;
	// Set LED pin to pin 8
	GPIO_Struct.GPIO_Pin=				GPIO_Pin_8;
	// Set LED pin speed to 50 MHz
	GPIO_Struct.GPIO_Speed=			GPIO_Speed_50MHz;
	
	//Initializes the LED 0 
	GPIO_Init(GPIOA,&GPIO_Struct);
	
	// Set LED pin to pin 2
	GPIO_Struct.GPIO_Pin=				GPIO_Pin_2;
	//Initializes the LED 1
	GPIO_Init(GPIOD,&GPIO_Struct);
	
	//Sets the selected data port bits.
	//close led
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	
	// Configure the pin as an input with pull-up resistor:
	GPIO_Struct.GPIO_Mode=GPIO_Mode_IPU;
	// Specify the pin to configure:
	GPIO_Struct.GPIO_Pin=GPIO_Pin_5;
	// Set the pin speed to 50 MHz:
	GPIO_Struct.GPIO_Speed=GPIO_Speed_50MHz;
	
	//Initializes the key 0
	GPIO_Init(GPIOC,&GPIO_Struct);
	
	// Specify the pin to configure:
	GPIO_Struct.GPIO_Pin=GPIO_Pin_15;
	//Initializes the key 1
	GPIO_Init(GPIOA,&GPIO_Struct);
}
static stack* key_push=(void*)0;
void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_Struct;
	NVIC_InitTypeDef NVIC_EXTI;
	
	//key 0 PC5
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);

	EXTI_Struct.EXTI_Line=EXTI_Line5;
	EXTI_Struct.EXTI_LineCmd=ENABLE;
	EXTI_Struct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_Struct.EXTI_Trigger=EXTI_Trigger_Falling;
	
	EXTI_Init(&EXTI_Struct);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_EXTI.NVIC_IRQChannel=EXTI9_5_IRQn ;
	NVIC_EXTI.NVIC_IRQChannelCmd=ENABLE;
	NVIC_EXTI.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_EXTI.NVIC_IRQChannelSubPriority=0x01;
	
	NVIC_Init(&NVIC_EXTI);
	//key 1 PA15
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	
	EXTI_Struct.EXTI_Line=EXTI_Line15;
	EXTI_Init(&EXTI_Struct);
	
	NVIC_EXTI.NVIC_IRQChannel=EXTI15_10_IRQn;
	NVIC_Init(&NVIC_EXTI);
	
	key_push=Init(key_push);
}

void TIM_BaseConfig(void)
{
	NVIC_InitTypeDef NVIC_EXTI;
	TIM_TimeBaseInitTypeDef TIM_BaseStructure;
	
	//TIM 6
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
  NVIC_EXTI.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_EXTI.NVIC_IRQChannelCmd = ENABLE;
  NVIC_EXTI.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_EXTI.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_Init(&NVIC_EXTI);
	
	TIM_BaseStructure.TIM_ClockDivision	=TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_CounterMode	=TIM_CounterMode_Up;
	TIM_BaseStructure.TIM_Period		=10000-1;
	TIM_BaseStructure.TIM_Prescaler		=1800-1;
	TIM_BaseStructure.TIM_RepetitionCounter	=0;
	TIM_TimeBaseInit(TIM6, &TIM_BaseStructure);
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6,ENABLE);
	
	//TIM 7
	NVIC_EXTI.NVIC_IRQChannel=TIM7_IRQn;
	TIM_BaseStructure.TIM_Prescaler=SPEED_25HZ;//2Hz
	TIM_TimeBaseInit(TIM7,&TIM_BaseStructure);
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);

	NVIC_Init(&NVIC_EXTI);
}

void deplayms(unsigned int ms)
{
	SysTick->LOAD = ms * 9000 - 1; // 72MHz / 8 = 4.5MHz
  SysTick->VAL = 0;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
void blink()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	deplayms(250);
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	deplayms(250);
	
}

void KEY_Checking(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

    static unsigned char count = 0;
    ++count;
    if (count % 2 == 1) {
      GPIO_SetBits(GPIOA, GPIO_Pin_8);
    } else {
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
     }
    }
}

//static unsigned char count=0;
//key 0
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line5)==RESET) return;
	EXTI_ClearFlag(EXTI_Line5);
	TIM_Cmd(TIM7,DISABLE);
	if(empty(key_push))
	{
		blink_led=blink;
		push(&key_push,0);
	}
	else if(top(key_push)==0)
	{
		blink_led=(void*)0;
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
		pop(&key_push);
		if(!empty(key_push))
		{
		switch (top(key_push))
		{
			case 0:
			{
				blink_led=blink;
				break;
			}
			case 1:
			{
				TIM_SetCounter(TIM7,0);
				TIM_Cmd(TIM7,ENABLE);
				break;
			}
		}
	}
	}
	else
	{
		blink_led=blink;
		push(&key_push,0);
	}
}

//key 1
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line15)==RESET) return;
	EXTI_ClearFlag(EXTI_Line15);
	
	blink_led=(void*)0;
	if(empty(key_push))
	{
		TIM_Cmd(TIM7,ENABLE);
		push(&key_push,1);
	}
	else if(top(key_push)==1)
	{
		TIM_Cmd(TIM7,DISABLE);
		TIM_SetCounter(TIM7,0);
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
		pop(&key_push);
		if(!empty(key_push))
		{
		switch (top(key_push))
		{
			case 0:
			{
				blink_led=blink;
				break;
			}
			case 1:
			{
				TIM_Cmd(TIM7,ENABLE);
				break;
			}
		}
	}
	}
	else
	{
		TIM_Cmd(TIM7,ENABLE);
		push(&key_push,1);
	}
	
}

void TIM6_IRQHandler(void) 
{
	if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

    static unsigned char count = 0;
    ++count;
    if (count % 2 == 1) {
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
    }
		else 
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    }
	}
}

void TIM7_IRQHandler(void)
{
	   if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {
       TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

       static unsigned char count = 0;
       ++count;
       if (count % 2 == 1) {
           GPIO_SetBits(GPIOA, GPIO_Pin_8);
       } else {
           GPIO_ResetBits(GPIOA, GPIO_Pin_8);
       }
   }
}