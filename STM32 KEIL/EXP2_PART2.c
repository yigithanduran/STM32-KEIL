#include "stm32f10x.h"
#include "stm32f10x_it.h"

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;



void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);
void configureTIM2(void);

int state = 0;
int i;
int zaman=0;

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE); 
	// Clock enable for pin A, B and AFIO
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Clock enable for TIM2
	
	configureGPIO();
	configureEXTI();
	configureTIM2();
  configureNVIC();

while(1){
	
		switch(state){
		
		case 0: // Red light for car and Green light for pedestirian
				
			while(zaman<=5){ // 5 second delay
			GPIO_ResetBits(GPIOB, GPIO_Pin_6); // Reset pin B6 (Yellow LED)
			GPIO_SetBits(GPIOB, GPIO_Pin_4); // Set pin B4 (Red LED car)
		GPIO_SetBits(GPIOB, GPIO_Pin_5); // Set pin B5 (Green LED pedestrian)
			 
			}
		GPIO_ResetBits(GPIOB, GPIO_Pin_4); // Reset pin B4 (Red LED car)
		GPIO_ResetBits(GPIOB, GPIO_Pin_5); // Reset pin B5 (Green LED pedestrian)
			 
			state =1;
		
		break;
		
		case 1:
		while (zaman>5 && zaman<=7){ // 2 second delay
			GPIO_SetBits(GPIOB, GPIO_Pin_6); // Set pin B6 (Yellow LED car)
		GPIO_SetBits(GPIOB, GPIO_Pin_7); // Set pin B7 (Red LED pedestrian)
		}
		
			GPIO_ResetBits(GPIOB, GPIO_Pin_6); // Reset pin B6 (Yellow LED car)
		GPIO_ResetBits(GPIOB, GPIO_Pin_7); // Reset pin B7 (Red LED pedestrian)
		
		state=2;
		break;
		
		case 2:
				while (zaman>7 &&zaman<=12){ // 5 second delay
		 GPIO_ResetBits(GPIOB, GPIO_Pin_6); // Reset pin B6 (Yellow LED car)
		 GPIO_SetBits(GPIOB, GPIO_Pin_8); // Set pin B8 (Green LED car)
		GPIO_SetBits(GPIOB, GPIO_Pin_7); // Set pin B7 (Red LED pedestrian)
				}
			 
			GPIO_ResetBits(GPIOB, GPIO_Pin_8); // Reset pin B8 (Green LED car)
		GPIO_ResetBits(GPIOB, GPIO_Pin_7); // Reset pin B7 (Red LED pedestrian)
			state=3;
		
			break;
		
		case 3:
			while (zaman>12&& zaman<=14){ // 2 second delay
			GPIO_SetBits(GPIOB, GPIO_Pin_6); // Set pin B6 (Yellow LED car)
		GPIO_SetBits(GPIOB, GPIO_Pin_7); // Set pin B7 (Red LED pedestrian)
			}
		
			GPIO_ResetBits(GPIOB, GPIO_Pin_6); // Reset pin B6 (Yellow LED car)
		GPIO_ResetBits(GPIOB, GPIO_Pin_7); // Reset pin B7 (Red LED pedestrian)
		
			zaman=0;
			state = 0;
			break;
			
}
	

}

}


void configureGPIO(void){
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_5 | GPIO_Pin_7; 
	// Configure pins of LEDs (B4, B6, B8, B5, B7)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // Configure pin of buton (A0)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Pull_Down Mode
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); // Configure external interrupt 
	//for A0 pin
	
}

void configureEXTI(void){
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; // Line0 because the pin of buton is A0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Trigger falling mode
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

void configureNVIC(void){
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; // EXTI0 because the pin of buton is A0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	
if (state==2 && zaman<11) // If cars traffic lights is green two seconds 
	                             //after a pedestrian press the button
		{
	   zaman=11;  // jumps to yellow
	}
		
	else if (state==2 && zaman >=11)
	{
zaman=13;
		
	}		
		
		
EXTI_ClearITPendingBit(EXTI_Line0); // Clear interrupt flag
}

void TIM2_IRQHandler(void)
{

	
	if (TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{  
	
		zaman++; // "zaman" increases by one every second
		
		
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update); // Clear interrupt flag
	}

}

void configureTIM2(void){
	
	TIM_TimeBaseStructure.TIM_Period = 1999; // 72000000/((1999+1)*(35999+1)) = 1 Hertz = 1 second
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	
}


