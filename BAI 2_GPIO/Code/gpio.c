#include "stm32f10x.h" // Device header
#include "stm32f10x_rcc.h" // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO


void delay(int time) {
for(int i = 0;i< time;i++) {}
}
void RCC_Config(){
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
}
void GPIO_Config(){
GPIO_InitTypeDef GPIOLed;
GPIOLed.GPIO_Pin = GPIO_Pin_13 ;
GPIOLed.GPIO_Mode = GPIO_Mode_Out_PP;
GPIOLed.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC,&GPIOLed);
}
void chaseLed(uint8_t loop){  // Nhay duoi led
uint16_t Ledval;
for(int j = 0; j < loop; j++)
{
Ledval = 0x0010; //0b0 0001 0000
for(int i = 0; i < 4; i++)
{
Ledval = Ledval << 1;
GPIO_Write(GPIOC, Ledval);
delay(1000000);
}
}
}

int main(){
RCC_Config();
GPIO_Config();
while(1){
GPIO_SetBits(GPIOC,GPIO_Pin_13);
delay(10000000);
GPIO_ResetBits(GPIOC,GPIO_Pin_13);
delay(10000000);
}
}