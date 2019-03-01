# AliOS Things SPI2配置

在配置SPI2的时候，折腾了好久，一直不成功，用示波器来测试波形一直没输出。刚开始的时候怀疑哪个宏定义没有开，一直在折腾。后面没成功，后面就改用了郑总推荐的模拟SPI的方式，经过调试最终总算有波形输出了。但在调试过程中我发现，配置引脚中还有一个变量没注意，这个就是引脚的复用功能，在进行SPI2的配置过程中，我没有注意到这个问题，后面对照了datasheet，才发现在IO的复用功能写填错了，刚开始的时候就直接用Ali丢过来的patch就直接用了，没注意到这个问题。

在developerkit开发板中的配置如下：

board\developerkit\Str\spi

```
/*board\developerkit\Str\spi*/
/* SPI2 init function */
void MX_SPI2_Init(void)
{

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;                       //  
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;                //全双工模式
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;                    // 数据长度
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;                  //时钟极性
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;                      //时钟相位
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;                       //SS 信号由硬件（ NSS 管脚）还是软件控制  
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;   //设置 SPI 波特率预分频值
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;                     //起始位是 MSB 还是 LSB
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;                     //帧格式 SPI motorola 模式还是 TI 模式
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;     //硬件 CRC 是否使能
  hspi2.Init.CRCPolynomial = 7;                               //CRC 多项式
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;             //CRC 校验的长度
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;                 //用来设置在连续传输时，是否允许 SPI 在两个连续数据间产生 NSS 脉冲
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
```

```
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    /**SPI1 GPIO Configuration    
    PA4     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
  else if(spiHandle->Instance==SPI2)
  {
    /**SPI2 GPIO Configuration    
    PD0     ------> SPI2_NSS
    PA9     ------> SPI2_SCK
    PC1     ------> SPI2_MOSI 
    PD3     ------> SPI2_MISO
    */
    __HAL_RCC_SPI2_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_SPI2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_SPI2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }

}

```

```
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA4     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if(spiHandle->Instance==SPI2)
  {
    /**SPI2 GPIO Configuration    
    PD0     ------> SPI2_NSS
    PA9     ------> SPI2_SCK
    PC1     ------> SPI2_MOSI 
    PD3     ------> SPI2_MISO
    */
    __HAL_RCC_SPI2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_3);
 }

} 
```

写完初始化之后在board\developerkit\aos\aoc_init.c中 进行初始化即可。

初始化完毕之后就可以调用发送和接收函数进行通信。

