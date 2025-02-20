// Chuong trinh nhay led pin13 tren stm32, tan suat nhay 100s/lan.
#include "stm32f10x.h" // Device header
#include "stm32f10x_rcc.h" // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h" // Keil::Device:StdPeriph Drivers:TIM
void RCC_Config()
{
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Cap clock cho TIM2, clock nay tan so 72MHz
}
void GPIO_Config()
{
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC,&GPIO_InitStruct);
}
void TIM_Config() // Cau hinh TIM de moi lan dem duoc 0.1ms, tuc tao thang chia 0.1ms 
{
TIM_TimeBaseInitTypeDef TIM_InitStruct;
TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // chia nho xung do CPU cap cho. o day chia 1
TIM_InitStruct.TIM_Prescaler = 7200 - 1; // Dem 7200 xung clock thi timer dem len 1. 7200 *1/72M = 100 us =0.1ms. 7200-1 do timer dem tu 0x0000
TIM_InitStruct.TIM_Period = 0xFFFF; // chon enough de ngat khong xay ra.
TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // Dem len
TIM_TimeBaseInit(TIM2,&TIM_InitStruct); // Nap cau hinh vao TIM2
TIM_Cmd(TIM2,ENABLE); // Cho phep TIM2 hoat dong
}
void delay_ms(uint32_t time)
{
TIM_SetCounter(TIM2,0); // set TIM2 bat dau dem tu 0
	while(TIM_GetCounter(TIM2)< time * 10) {} // khi nao dem den time*10 thoat khoi vong lap while. note : 10 x 0.1ms = 1s.
}
int main()
{
RCC_Config();
GPIO_Config();
TIM_Config();
while(1){
GPIO_ResetBits(GPIOC,GPIO_Pin_13);
delay_ms(100);
GPIO_SetBits(GPIOC,GPIO_Pin_13);
delay_ms(100);
}
}