/* Includes H files ----------------------------------------------------------*/
#include "main.h"

/* Includes HPP files --------------------------------------------------------*/
#include "Timer.hpp"
#include "Leds.hpp"
#include "L3GD20.hpp"
#include "LSM303DLHC.hpp"
#include "GyronavtPackage.hpp"
#include "ComPort.hpp"

// ----------------------------------------------------------------------------
//
// Standalone STM32F3 empty sample (trace via NONE).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

/* #global variables -----------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks; // structure used for setting up the SysTick Interrupt

// Unused global variables that have to be included to ensure correct compiling
// ###### DO NOT CHANGE ######
// ===============================================================================
__IO uint32_t TimingDelay = 0;                     // used with the Delay function
__IO uint8_t DataReady = 0;
__IO uint32_t USBConnectTimeOut = 100;
__IO uint32_t UserButtonPressed = 0;
__IO uint8_t PrevXferComplete = 1;
__IO uint8_t buttonState;
// ===============================================================================


/* Defines ------------------------------------------------------------------*/
#define IST_VECTORS_NUM     98

/* Typedef ------------------------------------------------------------------*/
typedef void (* const pHandler)(void);
typedef void (* __user_pHandler)(void);

/* Global variables ---------------------------------------------------------*/

extern pHandler __isr_vectors[];

// Собственная таблица прерываний
__attribute__((aligned(128)))    // Cortex-M4 требует выравнивание по 128 байт!
__user_pHandler __user_vector_table[IST_VECTORS_NUM] = {0};

// ----------------------------------------------------------------------------

// Стадии программы
enum class ProgramStages{InfiniteSending};

// ----------------------------------------------------------------------------

// Периферия
Timer3 timer3;          // Основной таймер, запускающий чтение и отправку данных 
Timer4 timer4;          // Таймер для мерцаний светодиодом
Leds leds;              // Светодиоды на плате
L3GD20 gyro_sensor;     // Встроенный гироскоп
LSM303DLHC acc_sensor;  // Встроенный датчик с акселерометром, магнитным и
                        // температурным датчиками

// ----------------------------------------------------------------------------

// Интерфейсы связи
ComPort com_port;

// ----------------------------------------------------------------------------

uint32_t tick_counter = 0;      // Счётчик тиков основного таймера

// ----------------------------------------------------------------------------


int main()
{
    __disable_irq();

    // Загрузим собственную таблицу прерываний
    for(uint8_t i = 0; i < IST_VECTORS_NUM; i++){
        __user_vector_table[i] = __isr_vectors[i];
    }

    SCB->VTOR = (uint32_t)__user_vector_table;

    __DSB();
    __ISB();

    __enable_irq();

	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);       //will end up in this infinite loop if there was an error with Systick_Config
    
    // ##########################

    GyronavtPackage gyronavt_package;               // Пакет данных в формате "Гиронавт"
    uint32_t current_tick = 0;                      // Текущий тик основного таймера
    auto stage = ProgramStages::InfiniteSending;    // Стадия программы

    // Инициализируем всё оборудования
    InitAll();             
    
    // Поморгаем светодиодами после успешной инициализации
    leds.Toggle_Leds();

    // Запустим таймеры
    timer3.Start();
    timer4.Start();

    // Основной цикл программы
    while (true)
    {
        switch (stage){
        case ProgramStages::InfiniteSending:
            // if (current_tick != tick_counter){
                leds.LedOn(LED9);

                current_tick = tick_counter;

                // Считаем показания датчика
                gyro_sensor.ReadData();
                acc_sensor.ReadData();
                
                // Обновим данные посылки
                gyronavt_package.UpdateData();

                leds.LedOff(LED9);

                // Отправим пакет данных по интерфейсам связи
                // com_port.SendPackage(gyronavt_package);
                
            // }

            break;
        }
    }
}

// -------------------------------------------------------------------------------
// Инициализация оборудования
void InitAll(){
    leds.Init();
    gyro_sensor.Init();
    acc_sensor.Init();
    // com_port.Init();

    // Настройка таймера для начала сбора данных
    timer3.TimPrescaler = Prescaller_10kHz;     // Частота - 10 кГц
    timer3.TimPeriod = 25;                      // Период генерации прерывания - 2,5 мс = 400Гц 
    timer3.callback_func = UserTIM3_IRQHandler;
    timer3.Init();   // Необходимо вызывать ПОСЛЕ установки параметров

    // Настройка таймера для мерцания светодиодами
    timer4.TimPrescaler = Prescaller_10kHz;     // Частота - 10 кГц
    timer4.TimPeriod = 20000;                   // Период генерации прерывания - 2 с
    timer4.callback_func = UserTIM4_IRQHandler;
    timer4.Init();   // Необходимо вызывать ПОСЛЕ установки параметров
}

// -------------------------------------------------------------------------------

void TIM3_IRQHandler(void)
{
    timer3.CallBack();
}

void UserTIM3_IRQHandler(){
    tick_counter += 1;
    leds.ChangeLedStatus(LED8);
}

// -------------------------------------------------------------------------------

void UserTIM4_IRQHandler(){
    leds.ChangeLedStatus(LED6);
    leds.ChangeLedStatus(LED7);
}

// -------------------------------------------------------------------------------

void UserEP3_OUT_Callback(uint8_t *buffer)
{
    buffer[0] = 0;
}

// -------------------------------------------------------------------------------

void Error_Handler(void)
{
    /* Turn LED10/3 (RED) on */
    STM_EVAL_LEDOn(LED10);
    STM_EVAL_LEDOn(LED3);
    while (1)
    {
    }
}

// Function to insert a timing delay of nTime
// ###### DO NOT CHANGE ######
void Delay(__IO uint32_t nTime)
{
    TimingDelay = nTime;

    while (TimingDelay != 0){}
    // for (int i = 0; i < 1000000; i++){}
}

// Function to Decrement the TimingDelay variable.
// ###### DO NOT CHANGE ######
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}

// Unused functions that have to be included to ensure correct compiling
// ###### DO NOT CHANGE ######
// =======================================================================
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
    return 0;
}

uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
    return 0;
}
// =======================================================================
