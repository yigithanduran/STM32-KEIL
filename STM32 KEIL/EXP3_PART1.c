#include "stm32f10x.h"
#include "delay.h" // Gerekli kütüphaneler eklendi

GPIO_InitTypeDef GPIO_InitStructure; 
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure; // Fonksiyonlarin çalisabilmesi için tanimlamalar yapildi

void configureGPIO(void);
void configureTIM2(void);
int state=0; // Ilk durumun case 0 olmasi icin state 0'a esitlendi

int main(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // TIM2 için clock aktiflestirildi
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // A pini ve alternatif fonksiyon için clock aktiflestirildi
	delayInit();
	configureGPIO();
  configureTIM2(); // Tanimlamalarin yapildigi fonksiyonlar çagrildi
	
	while (1)
	{
		
		switch (state)
		{
			case 0 : // LEDler kapali
				
					TIM_OCInitStructure.TIM_Pulse = 0; // Ilk durumda LEDler kapali olacagi icin pulse degerleri 0'a esitlendi
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 1. case'e gecildi
				{
				 state=1;
			   delayMs(150);
			  }
				
			break;
			
			case 1 : // Kirmizi LED low

					TIM_OCInitStructure.TIM_Pulse = 500; // Sadece kirmizi LED dusuk ayarda yanacagi için pulse 9000 yapildi
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 2. case'e gecildi
				{
			   state=2;
				 delayMs(150);
			  }
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 4. case'e geçildi
			  {
				 state=4;
			   delayMs(150);
				}
				
			break;

			case 2 : // Sari LED low
									
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 500; // Sadece sari LED dusuk ayarda yanacagi için pulse 9000 yapildi
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 3. case'e gecildi
				{
		     state=3;
				 delayMs(150);
				}
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 7. case'e geçildi
			  {
				 state=7;
			   delayMs(150);
			  }
				
			break;
			
			case 3 : // Yesil LED low
				
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =500; // Sadece yesil LED dusuk ayarda yanacagi için pulse 9000 yapildi
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0 ) // Eger 1. butona basilirsa 0. case'e gecildi
				{
				 state=0;
				 delayMs(150);
			  }
				
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 10. case'e geçildi
			  {
				 state=10;
				 delayMs(150);
			  }
			 
			break;
			
			case 4 : // Kirmizi LED medium
				
					TIM_OCInitStructure.TIM_Pulse = 3000; // Sadece kirmizi LED orta ayarda yanacagi icin pulse 18000 yapildi
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 2. case'e gecildi
				{
				 state=2;
				 delayMs(150);
			  }
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 5. case'e geçildi
			  {
				 state=5;
			   delayMs(150);
			  }
			 
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 1. case'e geçildi
			  {
				 state=1;
			   delayMs(150);
			  }
			 
			 	break;
			 
			 case 5 : // Kirmizi LED high
				 
					TIM_OCInitStructure.TIM_Pulse = 12000; // Sadece kirmizi LED yuksek ayarda yanacagi icin pulse 27000 yapildi
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 2. case'e gecildi
				{
			   state=2;
				 delayMs(150);
			  }
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 6. case'e geçildi
			 {
			  state=6;
			  delayMs(150);
			 }
			 
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 4. case'e geçildi
			 {
				state=4;
			  delayMs(150);
			 }
			 
			 break;
			 
			 case 6 : // Kirmizi LED full
				 
					TIM_OCInitStructure.TIM_Pulse = 36000; // Sadece kirmizi LED full ayarda yanacagi icin pulse 36000 yapildi
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 2. case'e gecildi
				{
			   state=2;
				 delayMs(150);
			  }
			
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 5. case'e geçildi
			  {
				 state=5;
			   delayMs(150);
				}
				
			 break;
			 
			 case 7 : // Sari LED medium
				
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 3000; // Sadece sari LED orta ayarda yanacagi icin pulse 18000 yapildi
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 3. case'e gecildi
				{
				state=3;
				delayMs(150);
			  }
				
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 8. case'e geçildi
			 {
				state=8;
			  delayMs(150);
			 }
			 
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 2. case'e geçildi
			 {
				state=2;
			  delayMs(150);
			 }
				 
		  break;
			 
			 case 8 : // Sari LED high
				
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 12000; // Sadece sari LED yuksek ayarda yanacagi icin pulse 27000 yapildi
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 3. case'e gecildi
				{
			   state=3;
			   delayMs(150);
			 }
				
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 9. case'e geçildi
			  {
				 state=9; 
			   delayMs(150);
				}
				
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 7. case'e geçildi
			  {
				 state=7;
			   delayMs(150);
				}
				
			break;
			 case 9 : // Sari LED full
				
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 36000; // Sadece sari LED full ayarda yanacagi icin pulse 36000 yapildi
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =0;
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0) // Eger 1. butona basilirsa 3. case'e gecildi
				 {
			    state=3;
			    delayMs(150);
			   }
				
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 8. case'e geçildi
			   {
				  state=8;
			    delayMs(150);
				 }
			
			 break;
					
			 case 10 : // Yesil LED medium
				 
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =3000; // Sadece yesil LED orta ayarda yanacagi icin pulse 18000 yapildi
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0 ) // Eger 1. butona basilirsa 0. case'e gecildi
				{
			   state=0;
			   delayMs(150);	
			 }
				
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 11. case'e geçildi
			  {
				 state=11;
			   delayMs(150);
			  }
				
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 3. case'e geçildi
			  {
				 state=3;
			   delayMs(150);
			  }
				
			break;
				
			 case 11 : // Yesil LED high
				 
				  TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =12000; // Sadece yesil LED yuksek ayarda yanacagi icin pulse 27000 yapildi
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0 ) // Eger 1. butona basilirsa 0. case'e gecildi
			   {
			    state=0;
				  delayMs(150);
			   }
					 
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)!=0) // Eger 2. butona basilirsa 12. case'e geçildi
			   {
				  state=12;
			    delayMs(150);
			   }
				 
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 10. case'e geçildi
			   {
				  state=10;
			    delayMs(150);
			   }
				 
			 break;
			 
			 case 12 : // Yesil LED high
				 
				  TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC1Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse = 0;
					TIM_OC2Init(TIM2, &TIM_OCInitStructure);
					TIM_OCInitStructure.TIM_Pulse =36000; // Sadece yesil LED full ayarda yanacagi icin pulse 36000 yapildi
					TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			 
		   if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)!=0 ) // Eger 1. butona basilirsa 0. case'e gecildi
				 {
			    state=0;
				  delayMs(150);
			   }
			
			 if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)!=0) // Eger 3. butona basilirsa 11. case'e geçildi
			   {
				 state=11;
			   delayMs(150);
			   }
				 
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
	
}