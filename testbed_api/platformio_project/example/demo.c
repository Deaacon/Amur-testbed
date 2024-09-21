
#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_timer16.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "led.h"


const int T_MIN = 544/2;
const int T_MAX = 2400/2;
const int T_PER = 20000;

Timer16_HandleTypeDef htimer16_1;s

/*
 * Данный пример демонстрирует работу с GPIO и PAD_CONFIG.
 * В примере настраивается вывод, который подключенный к светодиоду, в режим GPIO.
 */

void SystemClock_Config();
void GPIO_Init();
static void Timer16_1_Init();

int main()
{
    // Инициализация стенда
    SystemClock_Config();
    // UART_Init(UART_0, 278, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    GPIO_Init();
    Timer16_1_Init();

    // Ожидание для инициализации
    for (volatile int i = 0; i < 1000000; i++)
        ;

    // Вывод сообщения по UART
    // xprintf("\nProgram started!\n");

    // Счетчик, который будет выведен
    int n = 0;
    // Переменная, которая будет выведена
    int t = 0;
    // Переменная для считывания сигнала
    int state;

    while (1)
    {
        // Считывание пинов по очереди
        t = 0;
        state = HAL_GPIO_ReadPin(GPIO_2, GPIO_PIN_3);
        t |= state;
        state = HAL_GPIO_ReadPin(GPIO_2, GPIO_PIN_4);
        t |= state << 1;
        state = HAL_GPIO_ReadPin(GPIO_2, GPIO_PIN_5);
        t |= state << 2;
        state = HAL_GPIO_ReadPin(GPIO_2, GPIO_PIN_0);
        t |= state << 3;
        state = HAL_GPIO_ReadPin(GPIO_2, GPIO_PIN_2);
        t |= state << 4;

        // ШИМ
        int val = (1 - n % 4) * (T_MAX - T_MIN) / 1 + T_MIN;
        HAL_Timer16_StartPWM(&htimer16_1, T_PER, val);

        // Переключение светодиодов сдвигом
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_15, GPIO_PIN_LOW);
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_9,  GPIO_PIN_LOW);
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_8,  GPIO_PIN_LOW);
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_11, GPIO_PIN_LOW);

        if (n%5==0)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_15, GPIO_PIN_HIGH);
        else if (n%5==1)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_9,  GPIO_PIN_HIGH);
        else if (n%5==2)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_8, GPIO_PIN_HIGH);
        else if (n%5==3)
            HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_11, GPIO_PIN_HIGH);

        // Ожидание тактов и вывод числа
        for (volatile int i = 0; i < 100; i++)
        {
            // Вывод числа на дисплей
            if (t == 0)
            {
                int s = n%10 + n%10*10 + n%10*100 + n%10*1000;
                showNumber(s);
            }
            else
                showNumber(t);
        }
        n++;
    }
}

void SystemClock_Config(void) // Функция инициализации тактирования
{
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

void GPIO_Init() // Функция инициализации GPIO
{
    // Включение тактирования на GPIO
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    // Значение по умолчанию
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Выбор настроек входной группы GPIO 0
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Pin  = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                           GPIO_PIN_4 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

    // Выбор настроек выходной группы GPIO 1
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Pin  = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                           GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_6  | GPIO_PIN_7  |
                           GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |
                           GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);

    // Выбор настроек выходной группы GPIO 2
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Pin  = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  |
                           GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_6  | GPIO_PIN_7;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
}


void Timer16_1_Init()
{
    htimer16_1.Instance = TIMER16_1;

    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    htimer16_1.CountMode = TIMER16_COUNTMODE_INTERNAL;
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_64;
    htimer16_1.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;
    htimer16_1.Preload = TIMER16_PRELOAD_AFTERWRITE;
    htimer16_1.Filter.ExternalClock = TIMER16_FILTER_8CLOCK;
    htimer16_1.Filter.Trigger = TIMER16_FILTER_NONE;
    htimer16_1.EncoderMode = TIMER16_ENCODER_ENABLE;
    htimer16_1.Waveform.Enable = TIMER16_WAVEFORM_GENERATION_ENABLE;
    htimer16_1.Waveform.Polarity = TIMER16_WAVEFORM_POLARITY_INVERTED;

    HAL_Timer16_Init(&htimer16_1);
}
