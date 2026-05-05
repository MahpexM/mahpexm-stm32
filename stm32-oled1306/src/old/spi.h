/*
 * spi.h
 *
 *  Created on: Nov 6, 2022
 *      Author: hussamaldean
 */

#ifndef SPI_H_
#define SPI_H_

#include "stdint.h"

void OLED_SPI_Pins_Init();

void OLED_SPI_Configure();

void OLED_SPI_Write(char *data,uint32_t size);

void OLED_Select(void);

void OLED_Deselect(void);

void OLED_DataMode();

void OLED_CommMode();





#endif /* SPI_H_ */
