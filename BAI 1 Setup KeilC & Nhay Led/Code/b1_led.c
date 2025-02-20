#define RCC_APB2ENR *((unsigned int*) 0x40021018)
#define GPIOC_CRH   *((unsigned int*) 0x40011004)
#define GPIOC_ODR   *((unsigned int*) 0x4001100C)	

void delay(unsigned int time){
for (unsigned int i = 0; i < time ; i++){}
}
int main(){
	
	RCC_APB2ENR |= (1 << 4);
	GPIOC_CRH |= (3 << 20);
	GPIOC_CRH &= ~(3 << 22);
	while(1){
	GPIOC_ODR |= (1 << 13);	//Cap 3V cho chan PC13, LED tat
		delay(10000000);
	GPIOC_ODR &= ~(1 << 13); // Bat led, dua dien ap PC13 ve 0V
		delay(10000000);
	}
}
	