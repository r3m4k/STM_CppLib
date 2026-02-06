/* Includes H files ----------------------------------------------------------*/
#include "main.h"

/* Includes HPP files --------------------------------------------------------*/
#include "Consts.hpp"
#include "GPTimers.hpp"
#include "Leds.hpp"
#include "L3GD20.hpp"
#include "LSM303DLHC.hpp"
#include "GyronavtPackage.hpp"
#include "ComPort.hpp"
#include "USART.hpp"
#include "GpioPort.hpp"
#include "GpioPin.hpp"
#include "SimpleKalman3dFilter.hpp"
#include "SensorsKalmanParams.hpp"

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
#define MODE    RELEASE
// #define MODE    CALIBRATION

/* Typedef ------------------------------------------------------------------*/
typedef void (* const pHandler)(void);

/* Global variables ---------------------------------------------------------*/

extern pHandler __isr_vectors[];

// Собственная таблица прерываний
__attribute__((aligned(128)))    // Cortex-M4 требует выравнивание по 128 байт!
__user_pHandler __user_vector_table[IST_VECTORS_NUM] = {0};

// ----------------------------------------------------------------------------

// Стадии программы
enum class ProgramStages{InfiniteSending};

// Периферия
STM_CppLib::Leds leds;                          // Светодиоды на плате
STM_CppLib::L3GD20 L3GD20_sensor;               // Встроенный гироскоп
STM_CppLib::LSM303DLHC LSM303DLHC_sensor;       // Встроенный датчик с акселерометром,
                                                // магнитным и температурным датчиками

// Интерфейсы связи
STM_CppLib::ComPort com_port;
STM_CppLib::USARTx usart1;

// Используемые таймеры
STM_CppLib::STM_Timer::Timer3<send_package> timer3;   // Основной таймер, запускающий чтение и отправку данных 
STM_CppLib::STM_Timer::Timer4<[](){
    leds.ChangeLedStatus(LED6);
    leds.ChangeLedStatus(LED7);
}> timer4;   // Таймер для мерцания светодиодами LED6, LED7

// Настройка внешнего прерывания на PC1, которое будет программно инициироваться
STM_CppLib::STM_GPIO::GPIO_Pin_EXTI
    <STM_CppLib::STM_GPIO::GPIO_Port::PortC, GPIO_PinSource1, update_package_data> Pin_PC1;

// ----------------------------------------------------------------------------

// Делитель 50 подобран опытным путём
SimpleKalman3dFilter acc_filter(LSM303DLHC_acc_variance / 50, LSM303DLHC_acc_variance);
SimpleKalman3dFilter gyro_filter(L3GD20_gyro_variance   / 50, L3GD20_gyro_variance);
SimpleKalman3dFilter mag_filter(LSM303DLHC_mag_variance / 50, LSM303DLHC_mag_variance);

// Пакет данных в формате "Гиронавт"

#if MODE == RELEASE
    STM_CppLib::STM_Packages::GyronavtPackage gyronavt_package(
        &acc_filter.filtered_value, &gyro_filter.filtered_value, &mag_filter.filtered_value
    ); 
    
#elif MODE == CALIBRATION
    STM_CppLib::STM_Packages::GyronavtPackage gyronavt_package(
        &LSM303DLHC_sensor.acc_data, &L3GD20_sensor.gyro_data, &LSM303DLHC_sensor.mag_data
    ); 
#endif


uint32_t tick_counter = 0;      // Счётчик тиков основного таймера

// ----------------------------------------------------------------------------


int main()
{
    // ##########################

    // Загрузим собственную таблицу прерываний
    __disable_irq();

    // Скопируем исходную таблицу прерываний
    for(uint8_t i = 0; i < IST_VECTORS_NUM; i++){
        __user_vector_table[i] = __isr_vectors[i];
    }

    SCB->VTOR = (uint32_t)__user_vector_table;

    __DSB();
    __ISB();

    __enable_irq();

	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(true) {}     //will end up in this infinite loop if there was an error with Systick_Config
    
    // ##########################

    auto stage = ProgramStages::InfiniteSending;    // Стадия программы

    // Инициализируем всё оборудования
    InitAll();             
    
    // Поморгаем светодиодами после успешной инициализации
    leds.ToggleLeds();

    // Считаем показания датчиков до запуска таймеров, чтобы не отправлять нулевые данные
    L3GD20_sensor.ReadData();
    LSM303DLHC_sensor.ReadData();
    update_package_data();
    
    // Запустим таймеры
    timer3.Start();
    timer4.Start();

    // Основной цикл программы
    while (true)
    {
        switch (stage){
        case ProgramStages::InfiniteSending:

            leds.LedOn(LED9);

            // Считаем показания датчиков
            L3GD20_sensor.ReadData();
            LSM303DLHC_sensor.ReadData();

            // Отфильтруем показания с датчиков
            acc_filter.append_value(LSM303DLHC_sensor.acc_data);
            gyro_filter.append_value(L3GD20_sensor.gyro_data);
            mag_filter.append_value(LSM303DLHC_sensor.mag_data);
            
            // Обновим данные gyronavt_package в прерывании EXTI_Line1 

            /* Программная инициализация прерывания */
            EXTI_GenerateSWInterrupt(EXTI_Line1);
            
            leds.LedOff(LED9);

            break;
        }
    }
}

// -------------------------------------------------------------------------------
// Инициализация оборудования
void InitAll(){
    leds.Init();
    leds.LedsOn();

    L3GD20_sensor.Init();
    LSM303DLHC_sensor.Init();
    // com_port.Init();
    usart1.Init();
    Pin_PC1.InitPinExti();

    // Настройка таймера для начала сбора данных
    uint32_t tim3_period = 25 - 1;      // те на 25 тик таймер переполнится и вызовется прерывание
    timer3.Init(tim3_period);

    // Настройка таймера для мерцания светодиодами
    uint32_t tim4_period = 20000 - 1;   // срабатывание каждые 2 с
    timer4.Init(tim4_period);
}

// -------------------------------------------------------------------------------

// Функция для обновления данных в посылке gyronavt_package
void update_package_data(){
    gyronavt_package.UpdateData();
}

// -------------------------------------------------------------------------------

// Функция для отправки посылки gyronavt_package по COM порту
void send_package(){
    // Изменим состояние светодиода при отправке сообщения
    leds.ChangeLedStatus(LED8);

    // Обновим счётчик таймера и контрольную сумму перед отправкой
    gyronavt_package.UpdateTime(++tick_counter);
    gyronavt_package.UpdateControlSum();

    // Отправим посылку по com порту и usart1
    // leds.LedOn(LED4);
    // com_port.SendPackage(gyronavt_package);
    // leds.LedOff(LED4);

    // leds.LedOn(LED5);
    usart1.SendPackage(gyronavt_package);
    // leds.LedOff(LED5);
}

// -------------------------------------------------------------------------------

void UserEP3_OUT_Callback(uint8_t *buffer)
{
    buffer[0] = 0;
}

// -------------------------------------------------------------------------------

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // было прерывание от приемника
        __NOP();

    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET){ // было прерывание от передатчика
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){} // дожидаюсь завершения выдачи текущего байта и отключаю прерывания от выдачи
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
    USART_ClearITPendingBit(USART1, USART_IT_ORE);
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
