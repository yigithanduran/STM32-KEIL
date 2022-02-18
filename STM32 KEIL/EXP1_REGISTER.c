#include "stm32f10x.h"
#include "delay.h"

#define LED_OFF 1
#define BLINK2000 2
#define BLINK1000 3

uint8_t state= LED_OFF;

int main(void)
{
	
int count1=0,count2=0;
uint8_t buton1,buton2;	

RCC->APB2ENR |= (0x1 << 4); // Enable clock for PortC
GPIOC->CRH &= ~(0xF << 20); // Clear settings for PC13
GPIOC->CRH |= (0x2 << 20); // CNF:MODE (Output|Push-Pull|2MHz)
	
RCC->APB2ENR |= (0x1 << 2); // Enable clock for PortA
GPIOA->CRL &= ~(0xF << 22); 
GPIOA->CRL |= (0x2 << 22); 
	
GPIOA->CRL &= ~(0xF << 24); 
GPIOA->CRL |= (0x2 << 24); 

delayInit();
	
while(1)
	
	
{
	
	buton1=((GPIOA-> IDR &= 0x020)>>5);
	buton2=((GPIOA->IDR &= 0x040)>>6);
	
	if(buton1){
		count1++;}
	
	else if(buton2 && count1%2 != 0){
		count2++;}

	
				switch(state)
		{
			case LED_OFF:
				
           GPIOC->BRR |= (1 << 13);			
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
			
				GPIOC->BSRR |= (1 << 13);
	      delayMs(1000);
	      GPIOC->BRR |= (1 << 13);
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
				
				GPIOC->BSRR |= (1 << 13);
	      delayMs(500);
	      GPIOC->BRR |= (1 << 13);
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
				
				GPIOC->BSRR |= (1 << 13);
				
				if(buton1)
				{
				  state = BLINK2000;
				}
		    break;		
		}
		delayMs(10);
	}
	
   }
