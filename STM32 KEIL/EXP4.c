// Necessary libraries were added
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include <stdio.h>
#include "delay.h"


GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
USART_InitTypeDef USART_InitStructure;
I2C_InitTypeDef I2C_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;

// Definitions were made for the functions to work
void GPIO_Config(void);  		
void ADC_Config(void);
void TIM2_Config(void);
void USART_Config(void);
void UART_Transmit(char *string); 
void I2C_Config(void);				
void configureEXTI(void);

// Variables were defined
double adcValue; 
double ref=50;		
int time=0;
char alldata[20];
double transmitData=0;
uint16_t data=0;
uint16_t dataBuffer1;
uint16_t dataBuffer2;
uint16_t dataBuffer;

void USART1_IRQHandler(void){
	char data;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) // UART data receive
	{	
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); // Clear interrupt flag
	}
}

void TIM2_IRQHandler(void) {
	time++;	// Time counter 
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // Clear interrupt flag
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1); // Clear interrupt flag
	}
}


int main(void){

// Functions were called
GPIO_Config();  		
TIM2_Config();
ADC_Config();
USART_Config();
I2C_Config();	
delayInit();
configureEXTI();

	
	while(1){
	
		// Wait if busy
		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		// Generate START condition
		I2C_AcknowledgeConfig(I2C1, ENABLE);

		I2C_GenerateSTART(I2C1, ENABLE);
		while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
		// Send device address for read
		I2C_Send7bitAddress(I2C1, 0x90, I2C_Direction_Receiver);
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		// Read the first data
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		dataBuffer1 = I2C_ReceiveData(I2C1);
		// Disable ACK and generate stop condition
		I2C_AcknowledgeConfig(I2C1, DISABLE);
		I2C_GenerateSTOP(I2C1, ENABLE);
		// Read the second data
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		dataBuffer2 = I2C_ReceiveData(I2C1);
		
		adcValue=(int) ADC_GetConversionValue(ADC1);	// Reading potentiometer value Analog to Digital
		dataBuffer1 = dataBuffer1 << 1;
		dataBuffer2 = dataBuffer2 >> 7;
		dataBuffer = dataBuffer1 + dataBuffer2;
			
		
		
		if(time==100)	// 1 second
		{
			//Sending Data to PC via USART for every 1 second
			transmitData = dataBuffer*0.5;
			sprintf(alldata,"%2.2f\r\n",(transmitData));
			UART_Transmit(alldata);
			time = 0;
		}
		
		data = USART_ReceiveData(USART1); // Taking data from PC 
		
		if(data == '1'	&&	dataBuffer>ref)// If data is 1 and the temperature is higher than the reference
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1); // Set the LED
		}
		else if(data == '0'	||	dataBuffer<=ref)// If data is 0 or the temperature is smaller than the reference
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);// Reset the LED
		}
		
	}
}	

void GPIO_Config(void)
{	
	

	// Enabling clock for PortA, PortB, ADC1, I2C and Timer2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 |  RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	// Configuring A1 as output for LED 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configuring B0 as input for button
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;// Input pull down
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// Configuring A0 as input for Potentiometer
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;// Analog input
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure pins (SDA, SCL)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
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

void TIM2_Config(void)
{
	// Configuring timer interrupt 
TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1, ENABLE);
TIM_TimeBaseStructure.TIM_Period = 35999;		  
TIM_TimeBaseStructure.TIM_Prescaler = 19; // Frequency = 72000000/((35999+1)*(19+1)) = 100Hz 
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
TIM_Cmd(TIM2, ENABLE);


// Configuring NVIC for TIM2
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
}

void configureEXTI(void)// Configuring external interrupt
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);// B0 pin
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; // Line0 because pin is B0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // Rising mode
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// Configuring NVIC for EXTI0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;           
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // Priority number is 1 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


void ADC_Config(void)
{
		// Set ADC clock 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	// Configuring ADC 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // Independent mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // Continuous mode for continuous reading
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel=1; // Number of channel
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_7Cycles5); // Channel 0 because potentiometer pin is A0
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}

	void EXTI0_IRQHandler(void)
{
  
 ref = (adcValue/4096)*50; // Remapping of potentiometer value: 0-4050 to 0-50 celcius degrees
		
EXTI_ClearITPendingBit(EXTI_Line0); // Clear interrupt flag
}


void I2C_Config(void){
	// I2C configuration
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
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

void USART_Config(void){
	// USART settings
	USART_InitStructure.USART_BaudRate = 9600; // Baud rate is 9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8 bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // Stop bit is 1
	USART_InitStructure.USART_Parity = USART_Parity_No; // No parity number
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	// Enable data receive interrupt & USART1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}