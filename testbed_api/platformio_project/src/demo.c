
#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "led.h"

/*
 * Данный пример демонстрирует работу с GPIO и PAD_CONFIG.
 * В примере настраивается вывод, который подключенный к светодиоду, в режим GPIO.
 */

void SystemClock_Config();
void GPIO_Init();

int main() {
    // Инициализация стенда
    SystemClock_Config();
    GPIO_Init();
    UART_Init(UART_0, 278, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    // Ожидание для инициализации
    for (volatile int i = 0; i < 1000000; i++)
        ;

    // Вывод сообщения по UART
    xprintf("\nProgram started!\n");

    // Число, которое будет выведено
    int n = 0;
    while (1)
    {
        // Переключение светодиодов сдвигом
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_0, GPIO_PIN_LOW);
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_1, GPIO_PIN_LOW);
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_2, GPIO_PIN_LOW);
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_3, GPIO_PIN_LOW);
        if (n%5==0)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_0, GPIO_PIN_HIGH);
        else if (n%5==1)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_1, GPIO_PIN_HIGH);
        else if (n%5==2)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_2, GPIO_PIN_HIGH);
        else if (n%5==3)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_3, GPIO_PIN_HIGH);

        // Ожидание тактов и вывод числа
        for (volatile int i = 0; i < 20; i++) {
            // Вывод числа на дисплей
            showNumber(n);
        }
        n++;
    }
}

void SystemClock_Config(void) { // Функция инициализации тактирования
    // Значение по умолчанию
    PCC_InitTypeDef PCC_OscInit = {0};

    // Выбор источников тактирования
    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;

    // Делители частоты
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;

    // Поправочные коэффициенты
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;

    // Инициализация
    HAL_PCC_Config(&PCC_OscInit);
}

void GPIO_Init() { // Функция инициализации GPIO
    // Значение по умолчанию
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Включение тактирования на GPIO
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    // Выбор настроек входной группы GPIO 0
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Pin  = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                           GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

    // Выбор настроек выходной группы GPIO 1
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Pin  = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                           GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_6  | GPIO_PIN_7  |
                           GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |
                           GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
}
