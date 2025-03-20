#include "stm32f10x.h" // Device header
#include "stm32f10x_rcc.h" // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_adc.h" // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_tim.h" // Keil::Device:StdPeriph Drivers:TIM
#include "kalman.h"
void RCC_Config() {
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
void GPIO_Config() {
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void ADC_Config() {
ADC_InitTypeDef ADC_InitStruct;
ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; //Mode hoat dong doc lap
ADC_InitStruct.ADC_NbrOfChannel = 1; 					  // So luong kenh ADC =1
ADC_InitStruct.ADC_ScanConvMode = DISABLE;			// không dùng chế độ scan (chuyển đổi AD trên lần lượt 16 kênh ADC)
ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // khong chon kich hoat ADC Tu ngoai
ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // cho chep convert AD lien tuc
ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; // Don bit ve ben phai thanh ghi. Do AD dung do phan dai 12 bit,du lieu converted luu vao thanh ghi 16 bit=> nen phai don bit ve mot phia de luc doc du lieu tranh bi sai,
ADC_Init(ADC1, &ADC_InitStruct);
ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
ADC_Cmd(ADC1, ENABLE);
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
void TIM_Config() {
TIM_TimeBaseInitTypeDef TIM_InitStruct;
TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
TIM_InitStruct.TIM_Prescaler = 72 - 1; // tao thang dem 1 us cho moi lan TIM dem len 1.
TIM_InitStruct.TIM_Period = 0xFFFF;
TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
TIM_Cmd(TIM2, ENABLE);
}
void delay_us(uint32_t time) {
TIM_SetCounter(TIM2, 0);
while (TIM_GetCounter(TIM2) < time) {}// set TIM2 dem tu 0 -> time => thoat vong lap while.
}
uint16_t final = 0;
uint16_t val;
uint32_t sum = 0;
int main() {
RCC_Config();
GPIO_Config();
TIM_Config();
ADC_Config();
// do tin hieu dien ap thu duoc se bi nhieu, khong on dinh nhay nhot, nen phai dung cac bo loc de lam tin hieu dien ap on dinh lai
// co mot so bo loc pho bien la : moving average : lay trung binh, hoac bo loc Kalman : bo loc tinh toan gia dinh.
//SimpleKalmanFilter(0.1,0.01,1);
while (1) { 
sum = 0;
for (int i = 0; i < 10; i++) { //tạo bộ lọc moving average bằng lấy trung bình 10 giá trị đo được liên tiếp
//val = ADC_GetConversionValue(ADC1);
delay_us(100);
sum += val;
}
final = sum / 10;
delay_us(1000);
//final =updateEstimate(val);
}
}