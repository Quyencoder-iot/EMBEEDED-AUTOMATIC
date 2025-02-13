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
 
•	SPI_Mode: Quy định chế độ hoạt động của thiết bị SPI (Master or Slave)
 
•	SPI_Direction: Quy định kiểu truyền của thiết bị (FullDuplex, RxOnly,RX,TX)
 

•	SPI_BaudRatePrescaler: Hệ số chia clock cấp cho Module SPI (2,4,8,16,…,256-chia hệ số theo hàm mũ của 2)
 
•	SPI_CPOL: Cấu hình cực tính (Polary) của SCK . Có 2 chế độ:
-	SPI_CPOL_Low: Cực tính mức 0 khi SCK không truyền xung.
-	SPI_CPOL_High: Cực tính mức 1 khi SCK không truyền xung.
(Thường chọn mức CPOL mức 0)
 
•	SPI_CPHA: Cấu hình hoạt động ở pha (phase) nào của SCK. Có 2 chế độ:
-	SPI_CPHA_1Edge: Tín hiệu truyền đi ở cạnh xung đầu tiên (pha 1 lên).
-	SPI_CPHA_2Edge: Tín hiệu truyền đi ở cạnh xung thứ hai (pha 2 xuống)
 
•	SPI_DataSize: Cấu hình số bit truyền. 8 hoặc 16 bit.
 
•	SPI_FirstBit: Cấu hình chiều truyền của các bit là MSB hay LSB.
Ví dụ truyền data 8 bit data = 0x80 = 0b1000 0000. MSB = Most Significant Bit = 1 : Bit có trọng số cao nhất(bít đầu bên trái)  , LSB = Least Significant bit = 0 (bit có trọng số thấp nhất (bit đầu bên phải).
 
•	SPI_CRCPolynomial: Cấu hình số bit CheckSum cho SPI.
 
•	SPI_NSS: Cấu hình chân SS là điều khiển bằng thiết bị hay phần mềm.
 
•	Hàm SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data), tùy vào cấu hình datasize là 8 hay 16 bit sẽ truyền đi 8 hoặc 16 bit dữ liệu. Hàm nhận 2 tham số là bộ SPI sử dụng và data cần truyền.
Bản chất việc truyền là chính là ghi Data vào thanh ghi DR
 
•	Hàm SPI_I2S_ReceiveData(SPI_TypeDef* SPIx) trả về giá trị đọc được trên SPIx. Hàm trả về 8 hoặc 16 bit data
Bản chất hàm nhận là đọc Data từ thanh ghi DR ra.
 
•	Hàm SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG) trả về giá trị 1 cờ trạng thái trong thanh ghi của SPI. 
Trong phần cứng SPI có 1 bộ đệm Transmit Buffer chứa data trước khi truyền đi chính là thanh ghi DR, nếu dữ liệu chưa truyền đi mà ta ghi data mới vào thì data cũ sẽ bị mất đi, bởi vậy phải kiểm tra cờ trạng thái xem thanh ghi DR còn trống không trước khi truyền. Các cờ thường được dùng:
-	SPI_I2S_FLAG_TXE: Cờ báo truyền, cờ này sẽ set lên 1 khi truyền xong data trong buffer.
-	SPI_I2S_FLAG_RXNE: Cờ báo nhận, cờ này set lên 1 khi nhận xong data.
-	SPI_I2S_FLAG_BSY: Cờ báo bận,set lên 1 khi SPI đang bận truyền nhận.

