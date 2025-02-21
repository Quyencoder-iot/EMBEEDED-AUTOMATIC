#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#define TX_Pin GPIO_Pin_9
#define RX_Pin GPIO_Pin_10
#define UART_GPIO GPIOA  // Dinh nghi cong ra USART La GPIOA
#define BRateTime 104 // baudrate (9600 baud) = 9600 bit/s => delay truyen 1 bit = 104.16 us (lam tron =104)
void RCC_Config() {
	//Cap clock cho GPIOA, TIM2
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
void GPIO_Config() {
	//Cau hinh chan TX, RX cho GPIO, 
	//RX nhan tin hieu nen phai de che do Input-floating, khong co dien tro keo len hoac keo xuong
	//TX truyen du lieu, set che do Out_PP, co dien tro keo len muc cao, de tin hieu duoc on dinh
GPIO_InitTypeDef GPIOInitStruct;
GPIOInitStruct.GPIO_Pin = RX_Pin;
GPIOInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(UART_GPIO, &GPIOInitStruct);
GPIOInitStruct.GPIO_Pin = TX_Pin;
GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(UART_GPIO, &GPIOInitStruct); //Nap cau honh vao bien GPIOInitStruct
}
void TIM_Config() {
	//Cau hinh Timer
TIM_TimeBaseInitTypeDef TIM_InitStruct;
TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_Prescaler = 72 - 1; // cau hinh 1 us : Timer dem len 1 lan.
TIM_InitStruct.TIM_Period = 0xFFFF;
TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
TIM_Cmd(TIM2, ENABLE);
}
void delay_us(uint32_t time) {
	// Ham delay thoi gian
TIM_SetCounter(TIM2, 0); 
while (TIM_GetCounter(TIM2) < time) {}
}
void clock() {  //tao clock voi chu ky xung = 104 us (tuong ung 9600 baud)
delay_us(BRateTime); // = delay_us(104) = 104 us truyen 1 bit.
}
void UARTSoftware_Init() { 
GPIO_SetBits(GPIOA, TX_Pin);delay_us(1);// Ham set che doi Idle cho chan TX o muc cao
}
void UARTSoftware_Transmitt(char c) {
// Ham truyen data (char c)
GPIO_ResetBits(GPIOA, TX_Pin); // Keo chan TX xuong 0 cho bit bat dau, bat dau truyen data.
clock(); // delay 104us
// Truyen 8 bit du lieu (tu LSB sang MSB)
for (int i = 0; i < 8; i++) {
	// Su dung mat na Mask (1<<i):bx0000 00i0: vi tri thu i, bit set len mot. bit 1 lan luot dich tu bit LSB->MSB
	// ham dieu kien if(condition) = neu gia tri muc cao (1) thi thuc hien set TX_pin len 1, nguoc lai set TX_pin ve 0
	if (c & (1 << i)) { 	
GPIO_SetBits(GPIOA, TX_Pin); // Gui bit 1
} else {
GPIO_ResetBits(GPIOA, TX_Pin); // Gui bit 0
}
clock(); // Delay 104 us (9600 baud)
}
// stop bit- bit dung
GPIO_SetBits(GPIOA, TX_Pin); // Keo chan TX cao cho stop bit
clock(); // Doi theo thoi gian baudrate de hoan thanh bit dung
}
char UARTSoftware_Receive() {
	// Ham nhan data 
char c = 0;
	
	while (GPIO_ReadInputDataBit(GPIOA, RX_Pin) == 1); // wait Cho den khi RX_pin = 0, thoat vong lap. (start bit =0)
// Cho 1.5 baudrate de lay mau du lieu chinh giua bit data, de tranh noise o suon len va suon xuong cua bit
delay_us(BRateTime + BRateTime / 2);
// Doc 8 bit du lieu (LSB truoc)
for (int i = 0; i < 8; i++) {
if (GPIO_ReadInputDataBit(GPIOA, RX_Pin)) {
	c |= (1 << i); // mask (1<<i) : cho phep ghi bit thu i cua bien c len 1.
}
clock(); // Doi theo thoi gian cho bit tiep theo
}
// Cho bit dung (nno phai cao)
delay_us(BRateTime / 2); // Doi thoi gian 0.5 baudrate sau khi doc du lieu
return c;
}
char data[] = {'H','A','I'};
int main() {
RCC_Config();
GPIO_Config();
TIM_Config();
UARTSoftware_Init(); // set trang thai idle cho chan TX
while (1) {
for (int i = 0; i < 3; i++) {
UARTSoftware_Transmitt(data[i]);
delay_us(1000);
}
char received = UARTSoftware_Receive();
UARTSoftware_Transmitt(received);
}
}
