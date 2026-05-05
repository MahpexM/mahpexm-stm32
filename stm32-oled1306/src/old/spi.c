/*
 * spi.c
 *
 *  Created on: Nov 6, 2022
 *      Author: hussamaldean
 */

#include "spi.h"
#include "stm32f10x.h"

void OLED_SPI_Pins_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;   // GPIOA + SPI1

    // PA5 (SCK), PA6 (MISO), PA7 (MOSI) → CRL
    GPIOA->CRL &= ~(GPIO_CRL_MODE5  | GPIO_CRL_CNF5  |
                    GPIO_CRL_MODE6  | GPIO_CRL_CNF6  |
                    GPIO_CRL_MODE7  | GPIO_CRL_CNF7);

    // MODE5/6/7 = 10 → 2 MHz output
    // CNF5/6/7  = 10 → alternate function push‑pull (SPI AF)
    GPIOA->CRL |= GPIO_CRL_MODE5_1      |                             // 0x????2000
                  GPIO_CRL_MODE6_1      |                             // 0x???20000
                  GPIO_CRL_MODE7_1      |                             // 0x??200000
                  GPIO_CRL_CNF5_1       |                             // 0x?4000000
                  GPIO_CRL_CNF6_1       |                             // 0x40000000
                  0x00000000;           //

    // PA9 (CS/OLED_CS), PA10 (DC/OLED_RS) → CRH, выходы 2 MHz, push‑pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE9  | GPIO_CRH_CNF9  |
                    GPIO_CRH_MODE10 | GPIO_CRH_CNF10);

    // MODE9/10 = 10 → 2 MHz output
    // CNF9/10  = 00 → push‑pull
    GPIOA->CRH |= GPIO_CRH_MODE9_1      |
                  GPIO_CRH_MODE10_1;
}

void OLED_SPI_Configure(void)
{
    SPI1->CR1 = 0;

    // Master, full‑duplex, 8‑bit, MSB first, fPCLK2/2
    SPI1->CR1 |= SPI_CR1_MSTR;

    // SSM = 1, SSI = 1
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

    // Enable SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

void OLED_SPI_Write(char *data, uint32_t size)
{
    uint32_t i = 0;

    while (i < size) {
        while (!(SPI1->SR & SPI_SR_TXE))
            ;

        SPI1->DR = (uint8_t)data[i];
        i++;
    }

    while (SPI1->SR & SPI_SR_BSY)
        ;

    (void)SPI1->SR;
}

void OLED_Select(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR9;   // PA9 = 0
}

void OLED_Deselect(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS9;   // PA9 = 1
}

void OLED_DataMode(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS10;  // PA10 = 1
}

void OLED_CommMode(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR10;  // PA10 = 0
}