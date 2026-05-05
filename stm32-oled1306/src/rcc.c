#include "rcc.h"

int RccClockInit()
{
    // Enable HSE
    // Setting PLL
    // Enable PLL
    // Setting count wait cycles of FLASH
    // Setting AHB1,AHB2 prescaler
    // Switch to PLL
    
    uint16_t timeDelay;
    RCC->CR |= RCC_CR_HSEON;          // Запуск генератора
    
    for (timeDelay=0;;timeDelay++)    // Раскачка кварца (стабилизация процесса)
    {
    	if (RCC->CR & RCC_CR_HSERDY)  // Выход из цикла по завершении стабилизации HSE
    	    break;
    	if (timeDelay > 0x1000)
    	{
    	    RCC->CR &= ~RCC_CR_HSEON; // Отключение "стартера" по таймауту  (4096 итераций)
    	    return 1;
    	}
    }
    
    RCC->CFGR |= RCC_CFGR_PLLMULL9;   // PLL x9
    RCC->CFGR |= RCC_CFGR_PLLSRC;     // PLL source: HSE
    RCC->CR |= RCC_CR_PLLON;          // Enable PLL
    
    for (timeDelay=0;;timeDelay++)    // Ожидание захвата подстроечной частоты
    {
    	if (RCC->CR & RCC_CR_PLLRDY)  // Реализовано на уровне флагов регистров
    	    break;
    	if (timeDelay > 0x1000)       // Отключение по таймауту (4096 итераций)
    	{
    	    RCC->CR &= ~RCC_CR_HSEON;
    	    RCC->CR &= ~RCC_CR_PLLON;
    	    return 2;
    	}
    }
    
    // Подготовка трассы (Flash и шины)
    FLASH->ACR |= FLASH_ACR_LATENCY_2;    // Добавление двух тактов ожидания для предотвращения процессорных сбоев
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;     // APB1 prescaler=2 - Системная настройка, ловим каждый второй такт
    
    RCC->CFGR |= RCC_CFGR_SW_PLL;                       // Переключение ядра на PLL
    while ((RCC->CFGR & RCC_CFGR_SWS) != (0x02<<2)) {}  // Ожидание подтверждения
    RCC->CR &= ~RCC_CR_HSION;                           // Отключение внутреннего HSI за ненадобностью
    
    return 0;
}
