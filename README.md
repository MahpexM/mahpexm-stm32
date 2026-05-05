# mahpexm-stm32
Наработки по STM32

1) В **ветке cmake**
```
git clone --branch cmake https://github.com/MahpexM/mahpexm-stm32.git
```
- cmake-вариант с использованием Geany: для удобства навигации по коду нужно в панели инструментов выбрать **Проект -> Сгенерировать теги**.
Для генерации нужно выполнить из корня проекта команду
```
make
```
Прошивка производится с помощью команды
```
st-flash write output/main.bin 0x8000000
```
Либо запуска файла ./write.sh

2) В **ветке stm32duino**
```
git clone --branch stm32duino https://github.com/MahpexM/mahpexm-stm32.git
```
- используется библиотека Adafruit для Arduino IDE.
Сборка - через меню **Скетч -> Экспорт бинарного файла** панели инструментов Arduino IDE.
Прошивка:
```
st-flash --connect-under-reset write ssd1306_sketch_may04a.ino.BLUEPILL_F103C8.bin 0x08000000
```
