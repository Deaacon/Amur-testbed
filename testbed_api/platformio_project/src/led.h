
#include "mik32_hal_gpio.h"

// Пины для вывода
HAL_PinsTypeDef a  = GPIO_PIN_13;
HAL_PinsTypeDef b  = GPIO_PIN_4;
HAL_PinsTypeDef c  = GPIO_PIN_7;
HAL_PinsTypeDef d  = GPIO_PIN_6;
HAL_PinsTypeDef e  = GPIO_PIN_3;
HAL_PinsTypeDef f  = GPIO_PIN_12;
HAL_PinsTypeDef g  = GPIO_PIN_10;
HAL_PinsTypeDef p  = GPIO_PIN_14;
HAL_PinsTypeDef d1 = GPIO_PIN_5;
HAL_PinsTypeDef d2 = GPIO_PIN_1;
HAL_PinsTypeDef d3 = GPIO_PIN_0;
HAL_PinsTypeDef d4 = GPIO_PIN_2;

void delay() {
    for (volatile int i = 0; i < 8000; i++)
        ;
}

// определение разряда
void pickDigit(int x) {
    HAL_GPIO_WritePin(GPIO_1, d1, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d2, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d3, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d4, GPIO_PIN_HIGH);

    switch(x) {
        case 0:
            HAL_GPIO_WritePin(GPIO_1, d1, GPIO_PIN_LOW); //включаем d1
            break;
        case 1:
            HAL_GPIO_WritePin(GPIO_1, d2, GPIO_PIN_LOW); //включаем d2
            break;
        case 2:
            HAL_GPIO_WritePin(GPIO_1, d3, GPIO_PIN_LOW); //включаем d3
            break;
        default:
            HAL_GPIO_WritePin(GPIO_1, d4, GPIO_PIN_LOW); //включаем d4
            break;
    }
}

// очистка
void clearLEDs()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, p, GPIO_PIN_LOW);
}

// вывод 0
void zero()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_LOW);
}

// вывод 1
void one()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_LOW);
}

// вывод 2
void two()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_HIGH);
}

// вывод 3
void three()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_HIGH);
}

// вывод 4
void four()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_HIGH);
}

// вывод 5
void five()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_HIGH);
}

// вывод 6
void six()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_HIGH);
}

// вывод 7
void seven()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_LOW);
}

// вывод 8
void eight()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_HIGH);
}

// вывод 9
void nine()
{
    HAL_GPIO_WritePin(GPIO_1, a, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, b, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, c, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, d, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, e, GPIO_PIN_LOW);
    HAL_GPIO_WritePin(GPIO_1, f, GPIO_PIN_HIGH);
    HAL_GPIO_WritePin(GPIO_1, g, GPIO_PIN_HIGH);
}

// определение символа (цифры)
void pickNumber(int x) {
    switch(x) {
        default:
            zero();
            break;
        case 1:
            one();
            break;
        case 2:
            two();
            break;
        case 3:
            three();
            break;
        case 4:
            four();
            break;
        case 5:
            five();
            break;
        case 6:
            six();
            break;
        case 7:
            seven();
            break;
        case 8:
            eight();
            break;
        case 9:
            nine();
            break;
    }
}

void showNumber(int n)
{
    clearLEDs();
    pickDigit(0); //включаем первую цифру
    pickNumber(n%10); //значение тысячи
    delay(); //пауза 5мс

    clearLEDs();
    pickDigit(1); //включаем вторую цифру
    pickNumber(n%10); //значение сотни
    delay(); //пауза 5мс

    clearLEDs();
    pickDigit(2); //включаем третью цифру
    pickNumber(n%10); //значение десятки
    delay(); //пауза 5мс

    clearLEDs();
    pickDigit(3); //включаем четвертую цифру
    pickNumber(n%10); //значение единицы
    delay(); //пауза 5мс
}
