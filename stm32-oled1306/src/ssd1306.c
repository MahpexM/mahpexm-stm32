#include "ssd1306.h"
#include "sync.h"     // sleep(uint8_t iterations); // 577067 = 1 c

static uint8_t displayBuff[BUFFER_SIZE];                       // Буфер экрана


void SPI1Init()
{
    // 1. Включаем тактирование
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->AHBENR  |= RCC_AHBENR_DMA1EN;
    
    // 2. PA5-SCK, PA7-MOSI (AF Output PP 50MHz = 1011)
    GPIOA->CRL &= ~((0xF << (4*5)) | (0xF << (4*7)));	// Очистка
	GPIOA->CRL |= (0xB << (4*5)) | (0xB << (4*7));		// 0b1011 = MODE=11, CNF=10
	
    
    // 3. PA8(RES), PA9(CS), PA10(DC) - Output PP 50MHz = 0011 (CNF=00, MODE=11)
    GPIOA->CRH &= ~((0xF << 0) | (0xF << 4) | (0xF << 8));
	GPIOA->CRH |= (0x3 << 0) | (0x3 << 4) | (0x3 << 8);
    
    // 4. Инициализация состояний (HIGH по умолчанию)
    GPIOA->BSRR = (1<<8) | (1<<9) | (1<<10);  // RES=1, CS=1, DC=1
    
    // 5. SPI1 настройка
    SPI1->CR1 = 0;
    SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | 
                  SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
    SPI1->CR1 |= SPI_CR1_SPE;
    sleep(5000);
    
    // 6. DMA1 Channel 3
    DMA1_Channel3->CCR = 0;
    DMA1_Channel3->CCR |= (DMA_CCR3_DIR | DMA_CCR3_MINC | 
                          DMA_CCR3_PL_1 | DMA_CCR3_TCIE);
}

void SPI1Transmit(uint8_t data)
{
    // Ждем, пока буфер передачи станет пустым
    while(!(SPI1->SR & SPI_SR_TXE)); 
    // Отправляем данные
    SPI1->DR = data;
    // Ждем, пока последний бит физически покинет ножку MOSI
    while((SPI1->SR & SPI_SR_BSY));
}


void ssd1306RunDisplayUPD()
{
    DMA1_Channel3->CCR &= ~(DMA_CCR1_EN);         // Выключить DMA
    DMA1_Channel3->CPAR = (uint32_t)(&SPI1->DR);  // Занесем в DMA адрес регистра данных SPI1
    DMA1_Channel3->CMAR = (uint32_t)&displayBuff; // Адрес данных
    DMA1_Channel3->CNDTR = sizeof(displayBuff);   // Размер данных
    DMA1->IFCR &= ~(DMA_IFCR_CGIF3);
    CS_RES;                                       // Выбор устройства на шине
    DMA1_Channel3->CCR |= DMA_CCR1_CIRC;          // Циклический режим DMA
    DMA1_Channel3->CCR |= DMA_CCR1_EN;            // Включить DMA
}

void ssd1306StopDispayUPD()
{
    CS_SET;                                       // Дезактивация устройства на шине
    DMA1_Channel3->CCR &= ~(DMA_CCR1_EN);         // Выключить DMA
    DMA1_Channel3->CCR &= ~DMA_CCR1_CIRC;         // Выключить циклический режим
}

void ssd1306SendCommand(uint8_t command) {
    COMMAND;          // DC в 0
    CS_RES;           // Выбрать чип
    SPI1Transmit(command);
    CS_SET;           // Отпустить чип
}

void ssd1306Init()
{
    uint16_t i;
    
    // 1. Очистка буфера
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        displayBuff[i] = 0;
    }
    
    // 2. ПРАВИЛЬНЫЙ аппаратный сброс (datasheet)
    RESET_SET;        // RES = 1
    sleep(500);       // ≥3ms (500 итераций)
    
    RESET_RES;        // RES = 0 (импульс)
    sleep(10);        // ≥3μs (10 итераций достаточно)
    
    RESET_SET;        // RES = 1 навсегда!
    sleep(1000);      // ≥100μs стабилизации
    
    CS_SET;           // CS = 1 (неактивен)
    
    // 3. Инициализация командами (RES больше не трогаем!)
    ssd1306SendCommand(0xAE); // Display OFF
    ssd1306SendCommand(0x20); // Memory Addressing: Horizontal
    ssd1306SendCommand(0xB0); // Page 0 start
    ssd1306SendCommand(0xC8); // Scan direction
    ssd1306SendCommand(0xA1); // Segment remap
    ssd1306SendCommand(0xA8 | 0x3F); // Multiplex ratio 64-1
    ssd1306SendCommand(0xDA); ssd1306SendCommand(0x12); // COM pins
    ssd1306SendCommand(0x81); ssd1306SendCommand(0xCF); // Contrast
    ssd1306SendCommand(0xD5); ssd1306SendCommand(0x80); // Display clock
    ssd1306SendCommand(0xD9); ssd1306SendCommand(0xF1); // Precharge period
    ssd1306SendCommand(0xDB); ssd1306SendCommand(0x40); // VCOMH deselect
    ssd1306SendCommand(0xA4); // Entire display ON
    ssd1306SendCommand(0xA6); // Normal display (не inverse)
    ssd1306SendCommand(0xAF); // Display ON
}


void ssd1306DrawPixel(uint16_t x, uint16_t y, COLOR color){
    if ((x < SSD1306_WIDTH) && (y < SSD1306_HEIGHT) && (x >= 0) && (y >= 0))
    {
        uint16_t coord = x + (y / 8) * SSD1306_WIDTH;
    	if (color == WHITE)
      	{
      	    displayBuff[coord] |= (1 << (y % 8));
	}
	else if (color == BLACK)
	{
	    displayBuff[coord] &= ~(1 << (y % 8));
	}
    }
}

void ssd1306FillDisplay(COLOR color)
{
    uint16_t i;
    for(i = 0; i < SSD1306_HEIGHT * SSD1306_WIDTH; i++)
    {
        if(color == WHITE)
            displayBuff[i] = 0xFF;
	else if(color == BLACK)
	    displayBuff[i]=0;
    }
}

void updateAndSleep(uint32_t iterations) 
{
    ssd1306RunDisplayUPD();   // 1. Даем команду: «Отправить то, что нарисовали, на экран»
    sleep(iterations);        // 2. Даем процессору команду: «Подожди немного»
}

void ssd1306SpiralStep(COLOR color, uint32_t delayTime) {
    int top = 0;
    int bottom = SSD1306_HEIGHT - 1;
    int left = 0;
    int right = SSD1306_WIDTH - 1;

    while (left <= right && top <= bottom) {
        // Вправо
        for (int i = left; i <= right; i++) ssd1306DrawPixel(i, top, color);
        top++;
        updateAndSleep(delayTime);

        // Вниз
        for (int i = top; i <= bottom; i++) ssd1306DrawPixel(right, i, color);
        right--;
        updateAndSleep(delayTime);

        // Влево
        if (top <= bottom) {
            for (int i = right; i >= left; i--) ssd1306DrawPixel(i, bottom, color);
            bottom--;
            updateAndSleep(delayTime);
        }

        // Вверх
        if (left <= right) {
            for (int i = bottom; i >= top; i--) ssd1306DrawPixel(left, i, color);
            left++;
            updateAndSleep(delayTime);
        }
    }
}

