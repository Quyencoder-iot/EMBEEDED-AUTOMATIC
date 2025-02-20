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
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO,&GPIO_Struct);
	
	GPIO_Struct.GPIO_Pin = SPI_MISO;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO,&GPIO_Struct);
}


void SPI_Idle(){
	GPIO_WriteBit(SPI_GPIO, SPI_SCK, 0); //bitreset = 0
	GPIO_WriteBit(SPI_GPIO, SPI_CS, 1); // bitset = 1
	GPIO_WriteBit(SPI_GPIO, SPI_MISO, 0);
	GPIO_WriteBit(SPI_GPIO, SPI_MOSI, 0);
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

uint8_t SPI_Slave_Receive(void) {
    uint8_t dataReceive = 0x00;
    
    while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS)); // Wait chan CS o muc thap = 0, chi thoat duoc vong lap while khi dieu kien o muc thap
    for (int i = 0; i < 8; i++) {
        while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK)); //  // Wait SCK o muc cao
		
        if (GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI)) {
            dataReceive |= 1;  
        }
				dataReceive <<= 1;
				
        while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK)); // wait het bit 1 cua xung clock SCK
    }
    // Ðoi Stop khi CS len cao
    while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS));

    return dataReceive;
}

uint8_t ReceiveData;

int main(){
	RCC_config();
	GPIO_config();
	TIM_config();      
	SPI_Idle();        

	while(1)
	{
		ReceiveData = SPI_Slave_Receive(); 

	}
}