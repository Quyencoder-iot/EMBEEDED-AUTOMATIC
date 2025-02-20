#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"

#define SPI_CS GPIO_Pin_4
#define SPI_SCK GPIO_Pin_5
#define SPI_MISO GPIO_Pin_6
#define SPI_MOSI GPIO_Pin_7
#define SPI_GPIO GPIOA
#define SPI_RCC RCC_APB2Periph_GPIOA

void RCC_config(){
	RCC_APB2PeriphClockCmd(SPI_RCC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
}

void GPIO_config(){
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Pin = SPI_CS | SPI_SCK | SPI_MOSI;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO,&GPIO_Struct);
	
	GPIO_Struct.GPIO_Pin = SPI_MISO;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO,&GPIO_Struct);
}

void TIM_config(){
	TIM_TimeBaseInitTypeDef TIM_Struct;
	TIM_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Struct.TIM_Period = 0xFFFF;
	TIM_Struct.TIM_Prescaler = 7200 - 1; //0.1ms
	TIM_TimeBaseInit(TIM2, &TIM_Struct);
	TIM_Cmd(TIM2,ENABLE);
	
}
void delay_ms(uint16_t time){
	TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2) < time * 10);
}
//ham SCK dong bo tin hieu giua 2 vdk
void Clock()
{
	GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_SET);
	delay_ms(4);
	GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_RESET);
	delay_ms(4);
}

//SPI_IDle : Ham cai dat trang thai ban dau cua SPI (luc no trang thai nghi Idle)
void SPI_Idle(){
	GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_RESET);
	GPIO_WriteBit(SPI_GPIO, SPI_CS, Bit_SET);
	GPIO_WriteBit(SPI_GPIO, SPI_MISO, Bit_RESET); //khi chua co SCK thi MISO va MOSI bang bn cung duoc, vi luc do no chua doc 2 chan nay, ta co the mac dinh de no =0=reset
	GPIO_WriteBit(SPI_GPIO, SPI_MOSI, Bit_RESET);
}

// Ham truyen 8 bit du lieu u8Data, VDK thuc hien truyen tung buoc theo thu tu
// Keo chan CS = 0, truyen 1 bit, dich 1 bit, gui clock(): làm hàm lap 8 lan. Ghi chan cs len 1 de ket thuc truyen nhan
void SPI_Master_Transmit(uint8_t u8Data){ //ob1001 0000
	uint8_t u8Mask = 0x80; // ob1000 0000
	uint8_t tempData;
	GPIO_WriteBit(SPI_GPIO, SPI_CS, Bit_RESET);
	delay_ms(1);
	for(int i=0; i<8; i++){
		tempData = u8Data & u8Mask;
		if(tempData){
			GPIO_WriteBit(SPI_GPIO, SPI_MOSI, Bit_SET);
			delay_ms(10);
		} else{
			GPIO_WriteBit(SPI_GPIO, SPI_MOSI, Bit_RESET);
			delay_ms(10);
		}
		u8Data<<=1;
		Clock();
	}
	GPIO_WriteBit(SPI_GPIO, SPI_CS, Bit_SET);
	delay_ms(10);
}
uint8_t Datatrans[] = {1, 3, 9, 10, 15, 19, 90}; //data
int main(){
    RCC_config();           
    GPIO_config();           
    TIM_config();            
    SPI_Idle();             

    while(1){    
        for(int i = 0; i < sizeof(Datatrans)/sizeof(Datatrans[0]); i++){
            SPI_Master_Transmit(Datatrans[i]);            
            delay_ms(1000);                               
        }
    }
}