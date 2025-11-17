/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM_PORT_HPP
#define __COM_PORT_HPP

/* Includes ------------------------------------------------------------------*/
#include "I_CommunicationInterface.hpp"
#include "VCP_F3.h"
#include "hw_config.h"

/* Defines -------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
// Class comment
class ComPort: public I_CommunicationInterface{
public:

    ComPort() {}
    ~ComPort() {}

    // Инициализация COM порта
    void Init() override {
        VCP_ResetPort();    // Подтянули ножку d+ к нулю для правильной идентификации
        VCP_Init();         // Инициализация VCP
    }

    // Отправка по COM порту пакета данных
    void SendPackage(BasePackage& package){
        CDC_Send_DATA(package.data_ptr, package.len);
    }
};

#endif /*   __COM_PORT_HPP   */