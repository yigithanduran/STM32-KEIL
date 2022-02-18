#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
USART_InitTypeDef USART_InitStructure;

//Functions are defined
void GPIO_Config(void);
void ADC_Config(void);
void TIM2_Config(void);			
void UART_Transmit(char *string);
void EXTI_Config(void);		
void USART_Config(void);

//Variables are defined
double adcValue;
int time=0;
double button=0;
double out=0;
double c=0;
double data=0;
char adc[20];
int Sent_data=0;
int i=0;

// PI Controller
double PI(double r, double y)
{
 static double uk_1=0, ek_1=0;
 double ek = r - y;// Error
 double uk = uk_1 + 2.88*ek - 2.8* ek_1 ;// Difference equation for PI
 uk_1 = uk;
 ek_1 = ek;
 return uk;
}

// P Controller
double P(double r, double y)
{
 static double uk_1=0, ek_1=0;
 double ek = r - y;// Error
 double uk = uk_1 + 2.5 * ek - 2.5 * ek_1;// Difference equation for P
 uk_1 = uk;
 ek_1 = ek;
 return uk;
}

double Gz(double uk)
{
static double yk_1=0, yk_2=0, uk_1=0, uk_2=0;
double yk = 1.822*yk_1-0.8262*yk_2+0.002134*uk_1+0.002002*uk_2;
uk_2 = uk_1;
uk_1 = uk;
yk_2 = yk_1;
yk_1 = yk;
return yk;
}

void EXTI3_IRQHandler(void) //External interrupt handler
{ if(EXTI_GetITStatus(EXTI_Line3)!=RESET)
	{ 
	button=!button;
	i = 1800000;
	while(i>0){
		
		i--;
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) == 0)break;
	}
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void TIM2_IRQHandler(void) //Timer interrupt handler
{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) //Periodic IRQ
	{	time++;
		c =P(button,out); //Controller function are called
		TIM_OCInitStructure.TIM_Pulse = (c/3.3)*35999; //PWM Signal
		TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		adcValue = (double)(ADC_GetConversionValue(ADC1)); //ADC read
		out=(adcValue/4095)*3.3; //ADC output 
		//out=Gz(c); // For the Hardware-in-the-Loop part
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

int main(void)
{  
  //Functions are called
	GPIO_Config();
	ADC_Config();
	TIM2_Config();
	EXTI_Config();
	delayInit();
	USART_Config();
	
	while(1){
			
			adcValue = (double)(ADC_GetConversionValue(ADC1));
		
			if(time==1)// 100 ms
			{								
      sprintf(adc,"%f\r\n",out);
      UART_Transmit(adc);// Data is transmitted to PC
			time=0;}
		
	}

}
				
void GPIO_Config(void)
{	
	
	// Set ADC clock 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	// Enabling clock for PortA, ADC1, USART1, AFIO and Timer Interrupt
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 |  RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	
	// Configuring A2 for the Vin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configuring PA3 as input for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;// Pull-Down
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configuring PA4 as input for ADC Read
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;// Analog input
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configue UART RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configue UART TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

void USART_Config(void){
	// USART settings
	USART_InitStructure.USART_BaudRate = 2400;// Baud rate is 2400
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// 8 bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// Stop bit is 1
	USART_InitStructure.USART_Parity = USART_Parity_No;// No parity number
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	
	// Enable data receive interrupt & USART1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	
	// Configuring NVIC for USART
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;// No priority number for USART
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

	 void UART_Transmit(char *string)
{
	while(*string)
	{
		while(!(USART1->SR & 0x00000040));
		USART_SendData(USART1,*string);
		*string++;
	}
}

void EXTI_Config(void)
{
	
// Configuring external interrupt
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3); 
	EXTI_DeInit();
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;// Line3 because pin is A3
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// Configuring NVIC for external interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;// Priority number is 1 for EXTI3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	}

void ADC_Config(void)
{
	
	// Configuring ADC 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;// Independent Mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_7Cycles5);// Channel 4 because pin is A4
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}

void TIM2_Config(void)
{	
	
TIM_TimeBaseStructure.TIM_Period = 35999;	//72000000/(36000*200) = 10 Hz
TIM_TimeBaseStructure.TIM_Prescaler = 199;
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

// Configuring timer interrupt
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// PWM mode
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0;
TIM_OC3Init(TIM2, &TIM_OCInitStructure);
TIM_Cmd(TIM2, ENABLE);


// Configuring NVIC for timer interrupt
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;// No priority number for TIM2
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

}