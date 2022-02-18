#include "stm32f10x.h"
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure; 
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
ADC_InitTypeDef ADC_InitStructure;  
// Fonksiyonlarin �alisabilmesi i�in tanimlamalar yapildi

void configureGPIO(void); 
void configureTIM2(void);
void ADC_Config(void);

int a=0;
int state=0;
int adcValue;

int main(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Timer 2 i�in clock aktiflestirildi
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1, ENABLE); 
	//A pini ve alternatif fonksiyon i�in clock aktiflestirildi
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADC clock  ayarlandi (Maximum 14 MHz | 72/6 = 12 MHz)
	
	delayInit();
	configureGPIO();
  configureTIM2(); // Tanimlamalarin yapildigi fonksiyonlar �agrildi
	ADC_Config();
	
adcValue = (int)(ADC_GetConversionValue(ADC1)); // ADC den alinan deger degiskene esitlendi
	
	while (1)
	{
			adcValue= (int)(ADC_GetConversionValue(ADC1));
		// potansiyometre 0 ile 4095 arasinda deger d�nd�r�r. 4 farkli led parlakligi i�in bu deger 4 par�aya b�l�nd�.
	
		if (adcValue>=0 && adcValue<=1022)   // 0 ile 1022 arasinda d�nen deger i�in led parlakligi d�s�k
				{
          a=500;}
				
		  if (adcValue>=1022 && adcValue<=2045) // 1022 ile 2045 arasinda d�nen deger i�in led parlakligi orta
				{
          a=3000;}
				
			if (adcValue>=2045 && adcValue<=3067) //2045 ile 3067 arasinda d�nen deger i�in led parlakligi y�ksek
				{
          a=12000;}
				
				if (adcValue>=3067 && adcValue<=4090) // 3067 ile 4090arasinda d�nen deger ii�in led parlakligi full
				{
          a=36000;}
				
		switch (state)
		{
			case 0 :
				
					TIM_OCInitStructure.TIM_Pulse = 0;  // 0. durum t�m ledler kapali
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0)
				{
			    state=1;               // 4.pine bagli buton 1 ise durum 1 ye ge�ilsin
				  delayMs(150);
			 }
			break;
			
			case 1 :
				 //durum 1 kirimizi led yansin
	        TIM_OCInitStructure.TIM_Pulse = a; 
			// LED parlakligi ADC den alinan deger ile yukaridaki if bloguna bagli olark degisir
	        TIM_OC1Init(TIM2, &TIM_OCInitStructure);
          TIM_OCInitStructure.TIM_Pulse = 0;
	        TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	        TIM_OCInitStructure.TIM_Pulse =0;
	        TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0)
				{
			      state=2; // 4.pine bagli buton 1 ise durum 2 ye ge�ilsin
				    delayMs(150);
			 }
				break;

			
			case 2 :
				
	       TIM_OCInitStructure.TIM_Pulse = 0;
	       TIM_OC1Init(TIM2, &TIM_OCInitStructure);
         TIM_OCInitStructure.TIM_Pulse = a;
			// LED parlakligi ADC den alinan deger ile yukaridaki if bloguna bagli olark degisir
	       TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	       TIM_OCInitStructure.TIM_Pulse =0;
	       TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) 
				{
			  state=3;     // 4.pine bagli buton 1 ise durum 3 e ge�ilsin
			  delayMs(150);
			 }
			
			break;
			
			case 3 :
					TIM_OCInitStructure.TIM_Pulse = 0;
				  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =a;
			// LED parlakligi ADC den alinan deger ile yukaridaki if bloguna bagli olark degisir
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0 )
				{
			     state=0;  // 4.pine bagli buton 1 ise durum 0 a ge�ilsin
				   delayMs(150);
			 }
			
			  break;
			
		}
				
	}
	}

void configureGPIO(void) 
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;// LED pinleri tanimlandi (A0 A1 A2)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // PWM olarak kullanilcagindan AF olarak tanimlandi
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // buton pini tanimlandi (A4)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // Potansiyometre output i�in pin tanimlandi (A6)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // analog veri oldugu i�in AIN tanimlandi
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
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);// 1. LED'in pini A0 oldugu icin Channel 1 olarak ayarlandi
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);// 2. LED'in pini A1 oldugu icin Channel 2 olarak ayarlandi
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);// 3. LED'in pini A2 oldugu icin Channel 3 olarak ayarlandi

  TIM_Cmd(TIM2, ENABLE);// TIM2 aktif edildi

}