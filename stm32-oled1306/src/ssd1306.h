#ifndef __SSD1306
#define __SSD1306

#include "stm32f10x.h"
#include "stdlib.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define BUFFER_SIZE 1024

// Для PA8, PA9, PA10:
#define CS_SET      GPIOA->BSRR = (1 << 9)   // PA9 в 1
#define CS_RES      GPIOA->BSRR = (1 << 25)  // PA9 в 0 (9 + 16 = 25 бит для Reset)
#define DATA        GPIOA->BSRR = (1 << 10)  // PA10 в 1
#define COMMAND     GPIOA->BSRR = (1 << 26)  // PA10 в 0
#define RESET_SET   GPIOA->BSRR = (1 << 8)   // PA8 в 1
#define RESET_RES   GPIOA->BSRR = (1 << 24)  // PA8 в 0

/*
#define CS_SET GPIOA->BSRR|=GPIO_BSRR_BS2
#define CS_RES GPIOA->BSRR|=GPIO_BSRR_BR2
#define RESET_SET GPIOA->BSRR|=GPIO_BSRR_BS1
#define RESET_RES GPIOA->BSRR|=GPIO_BSRR_BR1
#define DATA GPIOA->BSRR|=GPIO_BSRR_BS3
#define COMMAND GPIOA->BSRR|=GPIO_BSRR_BR3
*/

typedef enum COLOR
{
    BLACK,
    WHITE
} COLOR;

void SPI1Init(void);
void SPI1Transmit(uint8_t data);
void ssd1306RunDisplayUPD(void);
void ssd1306StopDispayUPD(void);
void ssd1306SendCommand(uint8_t command);
void ssd1306Init(void);
void ssd1306DrawPixel(uint16_t x, uint16_t y,COLOR color);
void ssd1306FillDisplay(COLOR color);

void updateAndSleep(uint32_t iterations);
void ssd1306SpiralStep(COLOR color, uint32_t delayTime);
#endif
