#include <stdint.h>

void sleep(uint32_t x) {
    // Ключевое слово volatile не позволяет компилятору выкинуть пустой цикл
    volatile uint32_t i = x;
    while(i--)
        ;
}
