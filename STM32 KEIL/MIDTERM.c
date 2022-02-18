#include "stm32f10x.h"
#include "delay.h" // Gerekli kütüphaneler eklendi
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitStructure; 
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure; 
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
ADC_InitTypeDef ADC_InitStructure; 
USART_InitTypeDef USART_InitStructure; 

// Fonksiyonlarin çalisabilmesi için tanimlamalar yapildi

void ADC_Config(void);
void configureNVIC(void);
void configureEXTI(void);
void configureGPIO(void);
void configureTIM2(void);
void configureUSART(void);
void UART_Transmit(char *string);

int state=0; // Ilk durumun case 0 olmasi icin state 0'a esitlendi
int zaman=0;
int zaman2=0;
double data;
char adc[20];
int kirmizi = 0, sari = 0, yesil = 0;
int a=0;
int adcValue;
int hizkontrol=108000;




int main(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // TIM2 için clock aktiflestirildi
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1, ENABLE); // A pini ve alternatif fonksiyon için clock aktiflestirildi
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	delayInit();
	configureGPIO();
  configureTIM2(); // Tanimlamalarin yapildigi fonksiyonlar çagrildi
	configureNVIC();
	configureEXTI();
	ADC_Config();	
	configureUSART();
	
	while (1)
	{
		
		adcValue = (int)(ADC_GetConversionValue(ADC1));
		data = adcValue/40.95;
		sprintf(adc,"%f\r",data);
    UART_Transmit(adc);
		delayMs(10);
		
		switch (state)
		{
			case 0:
				
				TIM_OCInitStructure.TIM_Pulse = 0;  
				TIM_OC1Init(TIM2, &TIM_OCInitStructure); 
			TIM_OCInitStructure.TIM_Pulse = 0;  
				TIM_OC2Init(TIM2, &TIM_OCInitStructure); 
			TIM_OCInitStructure.TIM_Pulse = 0;  
				TIM_OC3Init(TIM2, &TIM_OCInitStructure); 
		
		if(zaman>=1) // Butona basildigi anda aktif olmasi icin zaman>=1 seklinde kullanildi
			{state=1;}
			
		else 
		{state=0;} 
			break;
		
		
			case 1:
				
			
			if(hizkontrol >36000)
			{
				if(zaman>=25 && yesil< 18000 ) 
		{
			yesil=yesil+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25  && yesil > 0) 
		{
			yesil=yesil-3600; 
			
		
				 
		 zaman2=0; // Zaman2 sayaci tekrar sifirlandi
		}
		
		else if(yesil==18000 && a ==5 ) // Eger yesil tam parlakliktaysa case 1'e geçilsin
		{ state=2;
		}
		else if(yesil <3600  ) // Eger yesil tam parlakliktaysa case 1'e geçilsin
		{ state=0;
		}
	}
		
	else if(hizkontrol < 36000)
			{
				if(zaman>=25 && yesil< hizkontrol ) 
		{
			yesil=yesil+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25  && yesil > 0) 
		{
			yesil=yesil-3600; 
			
		
				 
		 zaman2=0; // Zaman2 sayaci tekrar sifirlandi
		}
		
		else if(yesil==18000 && a ==5 ) // Eger yesil tam parlakliktaysa case 1'e geçilsin
		{ state=2;
		}
		else if(yesil <3600  ) // Eger yesil tam parlakliktaysa case 1'e geçilsin
		{ state=0;
		}
	}
	
			
			
			
		 TIM_OCInitStructure.TIM_Pulse = yesil;  // Yesil led pulse degeri guncellendi 
				TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		
		  break;
		
			case 2:
				
			if(hizkontrol>36000)
			{
		
				if(zaman>=25 && yesil < 36000 ) 
		{
			yesil=yesil+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && yesil> 18000) 
		{
			
			yesil=yesil-3600; 
			a=0;
			
		
				 
		 zaman2=0; 
		}
		
		else if(yesil==36000 && a ==5 ) 
		{ state=3;
		}
		else if(yesil==18000 && a ==5 ) 
		{ state=1;
		}
	}
				if(hizkontrol<36000)
			{
		
				if(zaman>=25 && yesil < hizkontrol ) 
		{
			yesil=yesil+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && yesil> 18000) 
		{
			
			yesil=yesil-3600; 
			a=0;
			
		
				 
		 zaman2=0; 
		}
		
		else if(yesil==36000 && a ==5 ) 
		{ state=3;
		}
		else if(yesil==18000 && a ==5 ) 
		{ state=1;
		}
	}
			
		 TIM_OCInitStructure.TIM_Pulse = yesil;  // Yesil led pulse degeri guncellendi 
				TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		
		break;
		
		
		
				case 3:
				
			if(hizkontrol >72000)
			{
				if(zaman>=25 && sari< 18000 ) 
		{
			sari=sari+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && sari > 0) 
		{
			sari=sari-3600; 
			a=0;
			
				 
		 zaman2=0; // Zaman2 sayaci tekrar sifirlandi
		}
		
		else if(sari==18000 && a ==5 ) 
		{ state=4;
		}
		else if(sari<3600 && a==5  ) 
		{ state=2;
		}
	}
		else	if(hizkontrol < 72000)
			{
				if(zaman>=25 && sari< hizkontrol-36000 ) 
		{
			sari=sari+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && sari > 0) 
		{
			sari=sari-3600; 
			a=0;
			
				 
		 zaman2=0; // Zaman2 sayaci tekrar sifirlandi
		}
		
		else if(sari==18000 && a ==5 ) 
		{ state=4;
		}
		else if(sari<3600 && a==5  ) 
		{ state=2;
		}
	}
		
		
			
		 TIM_OCInitStructure.TIM_Pulse = sari;  // Yesil led pulse degeri guncellendi 
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		
		  break;
		
			case 4:
		
			if(hizkontrol>72000)
			{
				if(zaman>=25 && sari < 36000 ) 
		{
			sari=sari+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && sari> 18000) 
		{
			
			sari=sari-3600; 
			a=0;
			
		
				 
		 zaman2=0; 
		}
		
		else if(sari==36000 && a ==5 ) 
		{ state=5;
		}
		else if(sari==18000 && a ==5 ) 
		{ state=3;
		}
	}
			
	
	
		else if(hizkontrol<72000)
			{
				if(zaman>=25 && sari < hizkontrol-36000 ) 
		{
			sari=sari+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && sari> 18000) 
		{
			
			sari=sari-3600; 
			a=0;
			
		
				 
		 zaman2=0; 
		}
		
		else if(sari==36000 && a ==5 ) 
		{ state=5;
		}
		else if(sari==18000 && a ==5 ) 
		{ state=3;
		}
	} 
			
	
		 TIM_OCInitStructure.TIM_Pulse = sari;  // Yesil led pulse degeri guncellendi 
				TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		
		break;
		
		
				case 5:
				
			if(hizkontrol<=120000)
			{
			
				if(zaman>=25 && kirmizi< hizkontrol-90000 ) 
		{
			kirmizi=kirmizi+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && kirmizi >0 ) 
		{
			kirmizi=kirmizi-3600; 
			
		
				a=0; 
		 zaman2=0; // Zaman2 sayaci tekrar sifirlandi
		}
		
		else if(kirmizi==18000 && a ==5 ) // Eger yesil tam parlakliktaysa case 1'e geçilsin
		{ state=6;
		}
		else if(kirmizi <3600 && a==5 ) // Eger yesil tam parlakliktaysa case 1'e geçilsin
		{ state=4;
		}
	}
		
			
			
			
		 TIM_OCInitStructure.TIM_Pulse = kirmizi;  // Yesil led pulse degeri guncellendi 
				TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		
		  break;
		
			case 6:
				if(hizkontrol<=120000)
				{
		
				if(zaman>=25 && kirmizi < hizkontrol-72000 ) 
		{
			kirmizi=kirmizi+3600; 
			 zaman=0;
			a=0;
			
		}
		
		  else if(zaman2>=25 && kirmizi> 18000) 
		{
			
			kirmizi=kirmizi-3600; 
			a=0;
			 
		 zaman2=0; 
		}
		
		else if(kirmizi==18000 && a ==5 ) 
		{ state=5;
		}
	}
		 TIM_OCInitStructure.TIM_Pulse = kirmizi;  // Yesil led pulse degeri guncellendi 
				TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		
		break;
	
		
		}	
	
	}
}

void configureGPIO(void) 
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2; // LED pinleri tanimlandi (A0,A1,A2)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // PWM olarak kullanilacagi icin AF olarak ayarlandi
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; // Buton pinleri tanimlandi (A3,A4,A5)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Pull down
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1; // Configure pin of buton (A0)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Pull_Down Mode
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0); 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);// Configure external interrupt 
	//for A0 pin
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // Potansiyometre output için pin tanimlandi (A6)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // analog veri oldugu için AIN tanimlandi
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
void ADC_Config(void)
{
	// ADC tanimlari yapildi 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // bagimsiz oalrak ayarlandi 
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,1,ADC_SampleTime_7Cycles5); 
	// potansiyometrede output 6.pine bagli  oldugu icin Channel 6 olarak ayarlandi
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1); //  kalibrasyon ayarlandi 
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // ADC aktif edildi
	}

void configureEXTI(void){
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; // Line0 because the pin of buton is A0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Trigger falling mode
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1; // Line0 because the pin of buton is A0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Trigger falling mode
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

void configureTIM2(void)
{
	TIM_TimeBaseStructure.TIM_Period = 35999; // 72000000/36000*20 = 100 Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 19;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
		
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC1Init(TIM2, &TIM_OCInitStructure); // 1. LED'in pini A0 oldugu icin Channel 1 olarak ayarlandi
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); // 2. LED'in pini A1 oldugu icin Channel 2 olarak ayarlandi
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure); // 3. LED'in pini A2 oldugu icin Channel 3 olarak ayarlandi

  TIM_Cmd(TIM2, ENABLE); // TIM2 aktif edildi
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
}

void TIM2_IRQHandler(void)
{

	
	if (TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
		
	{ 
		 if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))
		 {
         zaman++;
		 }
		 
		 if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4))
		 {
         zaman2++;
		 }
		
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		
	
	}

}
void EXTI0_IRQHandler(void)
{
  
	a=5;
		
EXTI_ClearITPendingBit(EXTI_Line0); // Clear interrupt flag
}
void EXTI1_IRQHandler(void)
{

	hizkontrol =adcValue*26.3;
	
	
	
EXTI_ClearITPendingBit(EXTI_Line1); // Clear interrupt flag
}



void configureNVIC(void){
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; // EXTI0 because the pin of buton is A0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; // EXTI0 because the pin of buton is A0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
}

void configureUSART(void){
	
		// USART settings
	USART_InitStructure.USART_BaudRate = 9600; // Baud rate degeri 9600 olarak ayarlandi
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 8 bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // Stop bit = 1
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	// Enable data receive interrupt & USART1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	
}

void UART_Transmit(char *string)
{
	while(*string)
	{	
		
//		while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
//		USART_SendData(USART1,*string++);
			while(!(USART1->SR & 0x00000040));
	    USART_SendData(USART1,*string);
	    *string++;
		
  }
}