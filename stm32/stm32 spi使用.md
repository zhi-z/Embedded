# stm32 spi使用

https://www.jianshu.com/p/5de187bf5b75



在ST官方代码中，SPI常见配置如下
**Init structure:**

hspi1.Instance = SPI1;
hspi1.Init.Mode = SPI_MODE_MASTER;
hspi1.Init.Direction = SPI_DIRECTION_2LINES;//配置SPI全双工，其实不知道ST怎么想的，这不是废话么
hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
/----------------------------------------------------------/
**hspi1.Init.NSS = SPI_NSS_SOFT;**//配置spi在master下，NSS作为普通IO，由用户自己写代码控制片选，可以1主多从
**hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;**//配置spi在master下，NSS作为SPI专用IO，由MCU自动控制片选，只能1主1从
**hspi1.Init.NSS = SPI_NSS_HARD_INPUT;**//仅当配置spi在slave下，作为从机片选输入
/----------------------------------------------------------/
hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
hspi1.Init.CRCPolynomial = 7;
hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

**Transmit sequence:**
SPI_NSS_HARD_OUTPUT模式下：

```
HAL_SPI_TransmitReceive( &hspi1, btx, brx, l, 5 ); // timeout 5msec;
while( hspi1.State == HAL_SPI_STATE_BUSY );  // wait for xmission complete
12
```

So I decided setting the pin manually using GPIO (using SPI_NSS_SOFT in init):
SPI_NSS_SOFT模式下：

```
HAL_GPIO_WritePin( NSS1_GPIO_Port, NSS1_Pin, GPIO_PIN_RESET ); // NSS1 low
HAL_SPI_TransmitReceive( &hspi1, btx, brx, l, 5 ); // timeout 5msec;
while( hspi1.State == HAL_SPI_STATE_BUSY );  // wait xmission complete
HAL_GPIO_WritePin( NSS1_GPIO_Port, NSS1_Pin, GPIO_PIN_SET ); // NSS1 high
```