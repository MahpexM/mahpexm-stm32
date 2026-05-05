#include "main.h"
#include "ssd1306.h"
#include "sync.h"

//~ OLED1306	GND		VDD		SCK		SDA		RES		DC		CS
//~ STM32		G		3.3		PA5  	PA7  	PA8		PA10	PA9 	
//~ Wire		коричн	красн	оранж	жёлт	жёлт	син		зелён

int main(void) {

    RccClockInit(); // Включение тактирования
    sleep(5000);
    SPI1Init();     // Инициализация шины SPI1    
    sleep(5000);
    
    // ТЕСТ DC ПИНА
    COMMAND;  // PA10 = 0
    sleep(5000); 
    DATA;     // PA10 = 1  
    sleep(5000);
    COMMAND;  // PA10 = 0
    sleep(5000);
    DATA;     // мигает PA10
    sleep(5000);
    
    ssd1306Init();  // Инициализация устройства OLED1306 на шине SPI
    sleep(5000);
    
    // Настройка прерываний DMA
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE; 
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    __enable_irq();
    sleep(5000);
    
    while (1) {
        // 1. Спираль появляется (белая нить)
        ssd1306SpiralStep(WHITE, 1000); 
        // Пауза, когда экран полностью заполнен
        sleep(5000); 
        // 2. Спираль исчезает (черная нить стирает белую)
        // Важно: если идти по тому же алгоритму, она будет исчезать 
        // с хвоста или с головы в зависимости от порядка.
        ssd1306SpiralStep(BLACK, 1000);
        // Пауза перед новым циклом
        sleep(5000);
    }
    
}
