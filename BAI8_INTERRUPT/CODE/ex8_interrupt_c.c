//CHUONG TRINH NHAN NUT PA0 DE NHAY LED PIN13
#include "stm32f10x.h" // Device header
#include "stm32f10x_rcc.h" // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_exti.h" // Keil::Device:StdPeriph Drivers:EXTI
#include "stm32f10x_tim.h" // Keil::Device:StdPeriph Drivers:TIM
void RCC_Config(){
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}
void GPIO_Config(){
GPIO_InitTypeDef GPIOInitStruct;
GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;     // Cau hinh pin 0 la input pull up
GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIOInitStruct);
GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // pin 13 la den led tren stm32 board, cau hinh la output push pull
GPIOInitStruct.GPIO_Pin = GPIO_Pin_13;
GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC, &GPIOInitStruct);
}
void NVIC_Config(){
NVIC_InitTypeDef NVICInitStruct;								// delcare 1 struct NVIC
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // Chon group 2
NVICInitStruct.NVIC_IRQChannel = EXTI0_IRQn; 		 	// chon line ngat EXTIO
NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Cap do uu tien chinh = 0: khong uu tien
NVICInitStruct.NVIC_IRQChannelSubPriority = 0x00; 				// Cap do uu tien phu = 0
NVICInitStruct.NVIC_IRQChannelCmd = ENABLE; 			// Lenh cho ngat hoat dong
NVIC_Init(&NVICInitStruct);} 	// Nap cau hinh cho NVICInitStruct

void EXTI_Config(){
EXTI_InitTypeDef EXTIInitStruct;
GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); // chon GPIOA, Chan PA0 lam nguon ngat
EXTIInitStruct.EXTI_Line = EXTI_Line0;	// do dung PA0 nen dung chuong trinh ngat ngoai Line0
EXTIInitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
EXTIInitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // ngat kich hoat luc bo nut nhan
EXTIInitStruct.EXTI_LineCmd = ENABLE;
EXTI_Init(&EXTIInitStruct);
}

void EXTI0_IRQHandler(void){
if (EXTI_GetITStatus(EXTI_Line0) != RESET) { //kiem tra flag ngat, neu dung Line0 moi thuc hien lenh
GPIOC->ODR ^= GPIO_Pin_13;
EXTI_ClearITPendingBit(EXTI_Line0);
}
}
void TIM_Config(){
TIM_TimeBaseInitTypeDef TIM_TimBaseInitStruct;
	TIM_TimBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimBaseInitStruct.TIM_Period = 10-1;
	TIM_TimBaseInitStruct.TIM_Prescaler = 7200-1;
	TIM_TimeBaseInit(TIM1,&TIM_TimBaseInitStruct);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
	
}
uint16_t count;
void delay(int time){
	count = 0; 
	while(count < time){ //Wait
}
}
void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)){ //Kiem tra update event da xay ra chua
		count++;	// Dem bien count them 1
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);// Clears the TIM2 interrupt pending bit
}
}
void USART_Config(){
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;

	USART_Init(USART1, &USART_InitStruct);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //Hàm cho phep ngat khi co flag bao nhan data USART_IT_RXNE
	USART_Cmd(USART1, ENABLE);
}
void USART1_IRQHandler()
{
	uint8_t data = 0x00;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)); // Cho co du lieu trong thanh ghi DR
		data = USART_ReceiveData(USART1);// Gan du lieu vao bien data
		if(USART_GetITStatus(USART1, USART_IT_TXE) == RESET){
			USART_SendData(USART1, data); // Tra du lieu ra man hinh
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}
	}
	USART_ClearITPendingBit (USART1, USART_IT_RXNE);
}


int main(){
RCC_Config();
GPIO_Config();
EXTI_Config();
USART_Config();
NVIC_Config();
while(1) {
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	//delay_ms(1000);
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	//delay_ms(1000);
}
}