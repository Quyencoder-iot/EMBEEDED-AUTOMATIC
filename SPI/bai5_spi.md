
Communication using SPI
4.1.Lý thuyết chung
SPI dùng 4 chân để giao tiếp giữa 2 thiết bị.
MASTER có thể giao tiếp với nhiều thiết bị SLAVE
Điều kiện để thực hiện giao tiếp được:
-	Chân SS được kéo xuống 0
-	Sau đó master phát xung SCK đồng bộ truyền nhận, lập trình truyền ở cạnh lên hoặc cạnh xuống của xung.
-	Master truyền Data đi trên chân chân MOSI (Master out Slave in)
-	Master nhận data phản hồi trên chân MISO (Master in, Slave out)
Trạng thái IDLE : chân SS = 0 và chân SCK =0.
Các kiểu truyền dữ liệu : 
-	Song công, data truyền 2 chiều trên đường dây MOSI va MISO bởi cả master và slave
-	Bán song công, chỉ truyền từ master tới slave
 

Đối với MASTER : 
SCK, MOSI, CS là Output => Cấu hình Mode output kiểu Push Pull 
MISO là input => Chọn Mode  input kiểu In-floating (điện áp thả nổi). thường đường đây được kéo lên mức 3v hay 5v bởi điện trở kéo để clear mức 1.
Với SLAVE ngược lại : 
SCK, MOSI, CS là Input => Chọn Mode input : In-Floating
MISO là output => Mode output, kiểu Push Pull 
Hàm Truyền dữ liệu
Hàm truyền dữ liệu sẽ lần lượt truyền 8 bit trong byte dữ liệu
-	Kéo CS = 0. 
-	Kiểm tra Clock() = 1 ??
-	Chỉ khi CS = 0 & SCK =1 thì đọc data trên chân MOSI ghi vào biến 
-	Dịch 1 bit
-	Kiểm tra CS = 1 => dừng đọc ghi

Hàm nhận dữ liệu
Hàm nhận dữ liệu sẽ lần lượt nhận 8 bit dữ liệu từ hàm truyền
-	Kiểm tra CS = 0 ?? . 
-	Kiểm tra Clock() = 1 ??
-	Chỉ khi CS = 0 & SCK =1 thì đọc data trên chân MOSI ghi vào biến 
-	Dịch 1 bit
-	Kiểm tra CS = 1 => dừng đọc ghi

2.Viết chương trình truyền dữ liệu giữa 2 MCU stm32, truyền mảng giữ liệu datatrans[]={3, 6, 9, 369, 999}
Phần cứng và thư viện cần dùng gồm
-	SPI (truyền nhận dữ liệu)
-	GPIO (config 4 chân cho SPI). Note: GPIO là cổng giao tiếp ra thế giới bên ngoài duy nhất của STM32. Sử dụng GPIO A thì cấp Clock qua bus APB2Periph
-	TIMER ( để tạo hàm delay và xung đồng bộ clock())
Có thể viết bằng Hardware và Software.
Các hàm đã có sẵn trong thư viện của vi điều khiển, để viết chương trình cần đi từ
Mỗi loại ngoại vi được định nghĩa thành 1 struct, nên search từ khóa “Typedef struct” để tìm hiểu cách tổ chức của các truct GPIO, TIMER. Truyền tham số và set thông số cho struct.
4.3. Lập trình truyền dữ liệu bằng HARDWARE
STM32 có 2 phần cứng SPI: SPI1 và SPI2
-	 SPI1 trên bus APB2, SPI 2 trên bus APB1.
-	Chân MOSI, MISO, SCK, NSS hoạt đông ở mode AF (Alternative Function)
 
Kiểm tra trong “pin definitions” table .Khi cấp clock cho SPI thì 4 chân đầu ra sẽ được kết nối mặc định đến các chân nào. như bảng dưới là NSS, SCK, MISO, MOSI lần lượt nối ra PA4,5,6,7 của GPIOA.
 
Nếu bật cùng lúc nhiều ngoại vi như ADC, SPI, USART cần remap lại các chân thì ta dùng thanh ghi AFIO_MAPR. 
Ví dụ set bit 0 của thanh ghi lên 1 thì ta đã remap chân của SPI 1 
0: No remap (NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7)
1: Remap (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)
 
 

Tương tự các ngoại vi khác, các tham số SPI được cấu hình trong Struct SPI_InitTypeDef:
 SPI_Mode: Quy định chế độ hoạt động của thiết bị SPI. 
●	SPI_Direction: Quy định kiểu truyền của thiết bị.
●	SPI_BaudRatePrescaler: Hệ số chia clock cấp cho Module SPI.
●	SPI_CPOL: Cấu hình cực tính của SCK . Có 2 chế độ:
○	SPI_CPOL_Low: Cực tính mức 0 khi SCK không truyền xung.
○	SPI_CPOL_High: Cực tính mức 1 khi SCK không truyền xung.
●	SPI_CPHA: Cấu hình chế độ hoạt động của SCK. Có 2 chế độ:
○	SPI_CPHA_1Edge: Tín hiệu truyền đi ở cạnh xung đầu tiên.
○	SPI_CPHA_2Edge: Tín hiệu truyền đi ở cạnh xung thứ hai.
●	SPI_DataSize: Cấu hình số bit truyền. 8 hoặc 16 bit.
●	SPI_FirstBit: Cấu hình chiều truyền của các bit là MSB hay LSB.
●	SPI_CRCPolynomial: Cấu hình số bit CheckSum cho SPI.
●	SPI_NSS: Cấu hình chân SS là điều khiển bằng thiết bị hay phần mềm.
●	Hàm SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data), tùy vào cấu hình datasize là 8 hay 16 bit sẽ truyền đi 8 hoặc 16 bit dữ liệu. Hàm nhận 2 tham số là bộ SPI sử dụng và data cần truyền.
●	Hàm SPI_I2S_ReceiveData(SPI_TypeDef* SPIx) trả về giá trị đọc được trên SPIx. Hàm trả về 8 hoặc 16 bit data.
●	Hàm SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG) trả về giá trị 1 cờ trong thanh ghi của SPI. Các cờ thường được dùng:
○	SPI_I2S_FLAG_TXE: Cờ báo truyền, cờ này sẽ set lên 1 khi truyền xong data trong buffer.
○	SPI_I2S_FLAG_RXNE: Cờ báo nhận, cờ này set lên 1 khi nhận xong data.
○	SPI_I2S_FLAG_BSY: Cờ báo bận,set lên 1 khi SPI đang bận truyền nhận. 
 
2.1.Logic lập trình bằng Software:
//1.Include thư viện gpio, rcc, tim, spi
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"

//2. Định nghĩa chân cho SPI, chọn cổng ra GPIOA, các chận PA4,5,6,7 đại diện cho 4 chân SPI
#define SPI_CS GPIO_Pin_4
#define SPI_SCK GPIO_Pin_5
#define SPI_MISO GPIO_Pin_6
#define SPI_MOSI GPIO_Pin_7
#define SPI_GPIO GPIOA
#define SPI_RCC RCC_APB2Periph_GPIOA // Dùng SPI, GPIOA nên cấp clock qua bus APB2Periph

void RCC_config(){
    RCC_APB2PeriphClockCmd(SPI_RCC,ENABLE);              // Cấp clock cho GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  // Cấp Clock cho timer 2, nằm trên bus APB1Periph
}

void GPIO_config(){
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.GPIO_Pin = SPI_CS | SPI_SCK | SPI_MOSI;  // Vs Master Các chân CS, SCK, MOSI đều là ouput, cùng mode và speed nên ghép chúng vào 1 cùng mảng pin
    GPIO_Struct.GPIO_Mode = GPGPIO_Mode_Out_PP;          // chế độ thả chôi điện áp
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;           // tốc độ truyền 50Hz
    GPIO_Init(SPI_GPIO,&GPIO_Struct);                    // Nạp các cấu hình trên cho biến GPIO_struct
    
    GPIO_Struct.GPIO_Pin = SPI_MISO;                     // Chân MISO là Input và khác mode nên cấu hình riêng
    GPIO_Struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO,&GPIO_Struct);                    // Nạp cấu hình cho riêng chân MISO.

// Để config cho GPIO trước tiên cần vào thư viện gpio.h, tìm từ khóa "typedef struct" xem cách cấu trúc set các thông số.
//Trong Struct GPIO cần cấu hình Pin, Mode, Speed

}

void SPI_Idle(){
    GPIO_WriteBit(SPI_GPIO, SPI_SCK, 0);  //bitreset = 0
    GPIO_WriteBit(SPI_GPIO, SPI_CS, 1);   // bitset = 1
    GPIO_WriteBit(SPI_GPIO, SPI_MISO, 0);
    GPIO_WriteBit(SPI_GPIO, SPI_MOSI, 0);
// KHi các chân ở trạng thái nghỉ Idle, cần reset các chân về mức logic nghỉ. 
}

void TIM_config(){
    TIM_TimeBaseInitTypeDef TIM_Struct;
    TIM_Struct.TIM_ClockDivision = TIM_CKD_DIV1;      // phép chia nhỏ xung clock. ở đây devided to 1.
    TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_Struct.TIM_Period = 0xFFFF;
    TIM_Struct.TIM_Prescaler = 7200 - 1;              //0.1ms
    TIM_TimeBaseInit(TIM2, &TIM_Struct);              // Nạp cấu hình cho TIM_Struct
    TIM_Cmd(TIM2,ENABLE);                             // Đối với TIMER cần hàm này để bật timer 2 cho phép hoạt động.
// Vào thư viện tim.h search từ khóa "typedef struct" để xem các cài đặt thông số cho timer struct.
}
void delay_ms(uint16_t time){
    TIM_SetCounter(TIM2,0);
    while(TIM_GetCounter(TIM2) < time * 10);
}

void Clock()
{
    GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_SET); // ghi bit 1 lên chân SCK và delay 4 ms
    delay_ms(4);
    GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_RESET); // Ghi bit 0 lên chân SCK và delay 4 ms
    delay_ms(4);
// Hàm clock để đồng bộ tín hiệu giữa 2 MCU
}

void SPI_Master_Transmit(uint8_t u8Data){   //ob1001 0000
    uint8_t u8Mask = 0x80;                  // ob1000 0000
    uint8_t tempData;                       // Tạo biến chứa dữ liệu tạm thời
    GPIO_WriteBit(SPI_GPIO, SPI_CS, Bit_RESET); // Ghi chân CS xuống mức thấp =0
    delay_ms(1);
    for(int i=0; i<8; i++){                      // Vòng lặp for thực hiện 8 lần để truyền 8 bit
        tempData = u8Data & u8Mask;              // mặt nạ u8Mask chỉ có bit MSB =1, các bit còn lại =0 nên phép and với u8Mask chỉ có vị trí bit MSB giữ nguyên giá trị,còn lại =0
        if(tempData){
            GPIO_WriteBit(SPI_GPIO, SPI_MOSI, Bit_SET);
            delay_ms(10);
        } else{
            GPIO_WriteBit(SPI_GPIO, SPI_MOSI, Bit_RESET);
            delay_ms(10);
        }
        u8Data<<=1;                             // Dịch 1 bit dữ liêu sang trái để tiếp tục phép & mặt nạ u8Mask
        Clock();                                // Sau khi dịch 1 bit thì truyền 1 clock đi
    }
    GPIO_WriteBit(SPI_GPIO, SPI_CS, Bit_SET);   // Ghi chan CS về mức thấp =0, kết thúc truyền
    delay_ms(10);
    // Nếu tempdata ở mức cao thực hiện ghi chân MOSI lên 1 (BIT_SET), ngược lại ghi cahan MOSI về 0 (BIT_RESET)
    // mỗi bước ghi thực hiện delay khoảng 10ms
    // Dịch u8Data 1 bit sang trái. để thực hiện phép & với bit MSB trong u8Mask.
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

Code Chương trình SLAVE nhận dữ liệu từ Master truyền sang
// Chương Trình SLAVE nhận mảng dữ liêu Datatrans[] từ MASTER truyền qua SPI.
// 1. include thư viện phần cứng sử dụng, gpio, rcc, timer, spi.

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"

//2. Định nghĩa chân cho SPI, chọn cổng ra GPIOA, các chận PA4,5,6,7 đại diện cho 4 chân SPI
#define SPI_CS GPIO_Pin_4
#define SPI_SCK GPIO_Pin_5
#define SPI_MISO GPIO_Pin_6
#define SPI_MOSI GPIO_Pin_7
#define SPI_GPIO GPIOA
#define SPI_RCC RCC_APB2Periph_GPIOA // Dùng SPI, GPIOA nên cấp clock qua bus APB2Periph

void RCC_config(){
    RCC_APB2PeriphClockCmd(SPI_RCC,ENABLE);              // Cấp clock cho GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  // Cấp Clock cho timer 2, nằm trên bus APB1Periph
}

void GPIO_config(){
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.GPIO_Pin = SPI_CS | SPI_SCK | SPI_MOSI;  // Vs SLave Các chân CS, SCK, MOSI đều là input, cùng mode và speed nên ghép chúng vào 1 cùng mảng pin
    GPIO_Struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;       // chế độ thả chôi điện áp
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;           // tốc độ truyền 50Hz
    GPIO_Init(SPI_GPIO,&GPIO_Struct);                    // Nạp các cấu hình trên cho biến GPIO_struct
    
    GPIO_Struct.GPIO_Pin = SPI_MISO;                     // Chân MISO là output của Slave và khác mode nên cấu hình riêng
    GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO,&GPIO_Struct);                    // Nạp cấu hình cho riêng chân MISO.

// Để config cho GPIO trước tiên cần vào thư viện gpio.h, tìm từ khóa "typedef struct" xem cách cấu trúc set các thông số.
//Trong Struct GPIO cần cấu hình Pin, Mode, Speed

}

void SPI_Idle(){
    GPIO_WriteBit(SPI_GPIO, SPI_SCK, 0);  //bitreset = 0
    GPIO_WriteBit(SPI_GPIO, SPI_CS, 1);   // bitset = 1
    GPIO_WriteBit(SPI_GPIO, SPI_MISO, 0);
    GPIO_WriteBit(SPI_GPIO, SPI_MOSI, 0);
// KHi các chân ở trạng thái nghỉ Idle, cần reset các chân về mức logic nghỉ. 
}

void TIM_config(){
    TIM_TimeBaseInitTypeDef TIM_Struct;
    TIM_Struct.TIM_ClockDivision = TIM_CKD_DIV1;      // phép chia nhỏ xung clock. ở đây devided to 1.
    TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_Struct.TIM_Period = 0xFFFF;
    TIM_Struct.TIM_Prescaler = 7200 - 1;              //0.1ms
    TIM_TimeBaseInit(TIM2, &TIM_Struct);              // Nạp cấu hình cho TIM_Struct
    TIM_Cmd(TIM2,ENABLE);                             // Đối với TIMER cần hàm này để bật timer 2 cho phép hoạt động.
// Vào thư viện tim.h search từ khóa "typedef struct" để xem các cài đặt thông số cho timer struct.
}
void delay_ms(uint16_t time){
    TIM_SetCounter(TIM2,0);
    while(TIM_GetCounter(TIM2) < time * 10);
}

void Clock()
{
    GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_SET);
    delay_ms(4);
    GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_RESET);
    delay_ms(4);
// Hàm clock để đồng bộ tín hiệu giữa 2 MCU
}

uint8_t SPI_Slave_Receive(void) {
    uint8_t dataReceive = 0x00;                             // khởi tạo biến dataReceive gán giá trị dầu =0x00

    while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS));        // Vong lap while điểm tra trạng thái chan CS liên tục, nếu CS ở mức cao =1 thì vòng lặp tiếp tục đến khi CS = 0 mức Low, nó sé thoát khỏi vòng lặp
    for (int i = 0; i < 8; i++) {                           // Thực hiện vòng lặp for 8 lần để ghi 8 bit dữ liệu
        while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK));  // trang thái Idle, SCK = 0 mức thấp. Đợi khi nào SCK =1 mức cao=> !SCK = 0 => thoát vòng lặp, thực hiện nhận dữ liệu
            
        if (GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI)) {    // Kiên tra chân MOSI ở mức cao thì thực hiện phép OR dataReceive với 1. ghi bit 1 vào biến
            dataReceive |= 1;  
        }
        dataReceive <<= 1;                                  // Dịch 1 bit để đợi ghi bit Data tiếp theo
        while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK));   // SCK đang mưc cao =1, Đợi khi nào SCK về mức thấp =0. Tức là đợi hết xung 1 của clock Nó sẽ thoát vòng lặp while.
    }
    while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS));       // Đợi CS lên mức cao, kết thúc vòng lặp, kết thúc nhận dữ liệu

    return dataReceive;
    // Hàm nhận SPI_Slave_Receive() có kiểu trả về 8 bit (uint8_t)
    // Hàm Receive hoạt động với điều khiên chân CS = 0, SCK = 1. dùng while để kiểm tra điều kiện 2 chân này.
    // Vòng lặp while(x), mặc địch tham số trong vòng lặp ở mức cao. chỉ thoát ra vòng lặp này tham số x có giá trị mức thấp = 0.
    // ví dụ while(1){} sẽ là vòng lặp vô tận
    // Hàm điều kiện if(x): nếu không có điều kiện cụ thể, mặc định x là mưc cao = 1 thì thực hiện điều kiện trong hàm.
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
