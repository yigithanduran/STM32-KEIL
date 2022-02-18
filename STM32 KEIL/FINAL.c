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

// Functuion Definitions 
void Clock_Config(void);
void GPIO_Config(void); 
void TIM2_Config(void);
void configureEXTI(void);
void NVIC_Config(void);
void ADC_Config(void);
void I2C_Config(void);
void USART_Config(void);
void UART_Transmit(char *string);
void pot(void);
void Recieve_I2C(void);
void Transmit_UART(void);
void On_off(void);
void Controller(void);
void check(void);
void Overshoot(void);
void Overshoot_led(void);

static double yk_1=0, yk_2=0, uk_1=0, uk_2=0,ek_1=0;
// Variables 
int potValue = 0;
double threshold=0;		
int time=0;
char alldata[20];
double transmitData=0;
uint16_t data=0;
uint16_t dataBuffer1;
uint16_t dataBuffer2;
uint16_t dataBuffer;
float ref=33;
float temp=0;
float diff;
double c=0;
double out=0;
float os=0;
float diff_os = 0;
float os_pc;

double PI(double r, double y) // PI controller
{
	
 static double uk_1=0,ek_1=0;
 double ek = r - y;// Error
 double uk = uk_1 + 0.1331353*ek - 0.1327* ek_1 ;// Difference equation for PI
 uk_1 = uk;
 ek_1 = ek;
 return uk;
}

double Gz(double uk) // G(z) function
{
static double yk_1=0, uk_1=0;
double yk = 0.9983*yk_1+0.02566*uk_1;
uk_1 = uk;
yk_1 = yk;
return yk;
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

	void EXTI0_IRQHandler(void)
{
  pot();
	
	os = 0;
 
	if (potValue <= 1365){ // If potantiometer value is smaller than 1365
		
		ref = 30; // Low reference
		
		GPIO_SetBits(GPIOA, GPIO_Pin_7); // Green LED is on
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	}
	
	else if (potValue > 1365 && potValue <= 2375){ // If potantiometer value is between 1365 and 2375
		
		ref = 40; // Medium reference
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		GPIO_SetBits(GPIOA, GPIO_Pin_5); // Yellow LED is on
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	}
	
	else if (potValue > 2375){ // If potantiometer value is higher than 2375
	
		ref = 45; // High reference
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		GPIO_SetBits(GPIOA, GPIO_Pin_6); // Red LED is on
	}
	
	
		
EXTI_ClearITPendingBit(EXTI_Line0); // Clear interrupt flag
}


int main(void){

//Functions are called
Clock_Config();
GPIO_Config();  		
TIM2_Config();
configureEXTI();
NVIC_Config();
ADC_Config();
I2C_Config();
USART_Config();
delayInit();



	while(1){
		
		Recieve_I2C();
		
		if(time == 100)	// 1 second
		{
			Transmit_UART(); // UART transmit function are called
			//Controller(); // Controller function are called
		}
		
		On_off(); // On_off function are called (For On_off mode)
		Overshoot(); // Overshoot function are called
		Overshoot_led(); // Overshoot_led function are called
		
	}
}	

void Clock_Config(void){

	// Set ADC clock 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	// Enabling clock for PortA, PortB, ADC1, I2C and Timer2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 |  RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

}
void GPIO_Config(void)
{	
	
	// Configuring A2, A3, A4, A5, A6, A7, A8 as output for LED (A8 for PWM control; A2, A3, A4 for overshoot; A5, A6, A7 for reference)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1; // Configuring A1 as PWM 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
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

// Configuring timer interrupt for PWM 	
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0; 
TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
TIM_Cmd(TIM2, ENABLE);
	
}

void configureEXTI(void)// Configuring external interrupt
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);// B0 pin
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; // Line0 because pin is B0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // Rising mode
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

void NVIC_Config(void){

	
  // Configuring NVIC for TIM2
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	// Configuring NVIC for EXTI0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;           
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
	
void ADC_Config(void)
{
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


void USART_Config(void){
	// USART settings
	USART_InitStructure.USART_BaudRate = 4800; // Baud rate is 4800
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8 bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // Stop bit is 1
	USART_InitStructure.USART_Parity = USART_Parity_No; // No parity number
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

}

void UART_Transmit(char *string) // Transmitting data to PC
{
	while(*string)
	{
		while(!(USART1->SR & 0x00000040));
		USART_SendData(USART1,*string);
		*string++;
	}
}

void pot() {
  uint16_t Pot = ADC_GetConversionValue(ADC1); // Reading potantiometer value
  potValue = (int)(Pot); 
}

void Recieve_I2C(void){

	// Wait if busy
		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		I2C_AcknowledgeConfig(I2C1,ENABLE);
		// Generate START condition
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
		
		// Settings for 0.25 resolution
		dataBuffer1 = dataBuffer1 <<2;
		dataBuffer2 = dataBuffer2 >>6;
		dataBuffer = dataBuffer1+dataBuffer2;
}

void Transmit_UART(void){

		//Sending Data to PC via USART for every 1 second
		temp = dataBuffer*0.25; // Temperature value
		sprintf(alldata,"%2.3f\r\n",(temp));
		UART_Transmit(alldata);
		time = 0;
}

void On_off(void){ // On_off function for on_off mode
	
		diff = ref - temp; // Difference between reference and temperature
	
		if(diff <= 0){ // If temperature is equal or higher than the reference
			
				TIM_OCInitStructure.TIM_Pulse = 0; // Pulse is 0 (system is off)
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
				GPIO_ResetBits(GPIOA, GPIO_Pin_8); // Reset the PWM control LED
			
		}
		
		else if (diff > 0){ // If temperature is smaller than reference
			
				TIM_OCInitStructure.TIM_Pulse = 35999; // Pulse is full (system is on)
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
				GPIO_SetBits(GPIOA, GPIO_Pin_8); // Set the PWM control LED
			
		}
}

void Controller(void){
	
	c =PI(ref,temp); 
		
	if(c<0){
		TIM_OCInitStructure.TIM_Pulse = 0; //PWM Signal
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	}
		
	else{ 
		
		if(c/3.59465 >= 1){
			TIM_OCInitStructure.TIM_Pulse = 35999; //PWM Signal
			TIM_OC2Init(TIM2, &TIM_OCInitStructure);
			GPIO_SetBits(GPIOA, GPIO_Pin_8);
			
		}
		else{
			TIM_OCInitStructure.TIM_Pulse = (c/3.59465)*35999; //PWM Signal
			TIM_OC2Init(TIM2, &TIM_OCInitStructure);
			GPIO_SetBits(GPIOA, GPIO_Pin_8);
		}
	}
	
	out = Gz(c);
}

void Overshoot(void){
	
	diff_os = temp - ref; // Difference between temperature and reference
	
	if (diff_os >= 0 ){
		
		if (diff_os>os){
			
			os = diff_os; // Overshoot is equated to maximum value of difference between temperature and reference
		
		}
	}
}

void Overshoot_led(void){
	
	os_pc = (os/ref)*100;
	
	if (os == 0){ // If overshoot is 0, reset all LEDs
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	}
	
	else{
		
		if (os_pc <=2){ // If overshoot is smaller than %2
		
		GPIO_SetBits(GPIOA, GPIO_Pin_4); // Set green LED
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		}
	
		else if (os_pc > 2 && os_pc <= 10 ){ // If overshoot is between %2 and %10 
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_3); // Set yellow LED
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		}
		
		else if(os_pc > 10){ // If overshoot is higher than %10
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		GPIO_SetBits(GPIOA, GPIO_Pin_2);	// Set red LED
		}
	}
}

