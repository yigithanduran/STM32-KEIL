#include "stm32f10x.h"
#include "delay.h"

#define LED_OFF 1
#define BLINK2000 2
#define BLINK1000 3
GPIO_InitTypeDef GPIO_InitStructure;
uint8_t state= LED_OFF;

int main(void)
{
	
int count1=0,count2=0;
uint8_t buton1,buton2;	
	
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(GPIOC, &GPIO_InitStructure);
	
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
GPIO_Init(GPIOA, &GPIO_InitStructure);
	
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
GPIO_Init(GPIOA, &GPIO_InitStructure);

delayInit();
	
while(1)
{
	
	buton1=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);
	buton2=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
	
	if(buton1){
		count1++;}
	
	else if(buton2 && count1%2 != 0){
		count2++;}

				switch(state)
		{
			case LED_OFF:
				
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
				delayMs(1000);
			
			if (count1%2==0){
				
				state = LED_OFF; 
			}
				
			else if(count1%2==1)
				{
					state = BLINK2000;
				}

				 break;
	
			case BLINK2000:
			
GPIO_SetBits(GPIOC, GPIO_Pin_13);
				delayMs(1000);
	      GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	      delayMs(1000);
			
			  if(count1%2==1 & count2%2==1)
				{
				  state = BLINK1000;
				}
				
				else if(count1%2==0)
				{
				  state = LED_OFF;
				}
				
				break;		

			case BLINK1000:
				
GPIO_SetBits(GPIOC, GPIO_Pin_13);
				delayMs(500);
	      GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	      delayMs(500);
			
			if(count1%2==1 & count2%2==0)
				{
				  state = BLINK2000;
				}
				
				else if(count1%2==0)
				{
				  state = LED_OFF;
				}
				
				break;

			default:
				
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				
				if(buton1)
				{
				  state = BLINK2000;
				}
		    break;		
		}
		delayMs(10);
	}
	
 }
