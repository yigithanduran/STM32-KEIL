#include "stm32f10x.h"
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);

int state=0;
int i;

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	//Enable clocks for pin A, B and Interrupt
	
	configureGPIO();
	configureEXTI();
	configureNVIC();
	delayInit();

while(1){
	
	
	switch(state){
		
		case 0: //Red LED
			
			GPIO_ResetBits(GPIOB, GPIO_Pin_6); //Reset pin B6 (Yellow LED)
			GPIO_SetBits(GPIOB, GPIO_Pin_4); //Set pin B4 (Red LED)
			delayMs(5000);
			GPIO_ResetBits(GPIOB, GPIO_Pin_4); //Reser pin B4 (Red LED)
			delayMs(10);
			 
			state =1;
		
		break;
		
		case 1: //Yellow LED
		
			GPIO_SetBits(GPIOB, GPIO_Pin_6); //Set pin B6 (Yellow LED)
			delayMs(2000);
			GPIO_ResetBits(GPIOB, GPIO_Pin_6); //Reset pin B6 (Yellow LED)
			delayMs(10);
		
		state=2;
		break;
		
		case 2: //Green LED
				
		 GPIO_ResetBits(GPIOB, GPIO_Pin_6);
		 GPIO_SetBits(GPIOB, GPIO_Pin_8); //Set pin B8 (Green LED)
		
		for ( i=0; i<500; i++) // Divide the delay and check if button is pressed in each loop
		{ 
			 delayMs(10);
			
			if(state!=2)
				{
				break;
			}	
		}
			 
			GPIO_ResetBits(GPIOB, GPIO_Pin_8); //Reset pin B8 (Green LED)
			state=3;
		
			break;
		
		case 3: //Yellow LED
			
			GPIO_SetBits(GPIOB, GPIO_Pin_6); // Set pin B6 (Yellow LED)
			delayMs(2000);
			GPIO_ResetBits(GPIOB, GPIO_Pin_6); // Reset pin B6 (Yellow LED)
			delayMs(10);
			state = 0;
			break;
			
}

}
}

void configureGPIO(void){
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_8; 
	// Configure pins of LEDs (B4, B6, B8)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // Configure pin of buton (A0)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Pull_Down Mode
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); 
	// Configure external interrupt for A0 pin
	
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
	// Priority number (This is necessary when there are more than 1 interrupt)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void EXTI0_IRQHandler(void)
{
	
if(EXTI_GetITStatus(EXTI_Line0) != RESET) // If A0 (buton) is set
{
	
	if (state==2) // If green LED is set
		{
	   GPIO_ResetBits(GPIOB, GPIO_Pin_8); // Reset green LED
		 GPIO_SetBits(GPIOB, GPIO_Pin_6); // Set yellow LED
		 state=0; // Return to the beginning of the loop (Set red LED)
	}
		
EXTI_ClearITPendingBit(EXTI_Line0); // Clear interrupt flag
}

}
