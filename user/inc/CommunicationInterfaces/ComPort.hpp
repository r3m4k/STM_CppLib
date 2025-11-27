/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COM_PORT_HPP
#define __COM_PORT_HPP

/* Includes ------------------------------------------------------------------*/
#include "VCP_F3.h"
#include "hw_config.h"

/* Defines -------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
namespace STM_CppLib{

    // Class comment
    class ComPort{
    public:

        ComPort() {}
        ~ComPort() {}

        // Инициализация COM порта
        void Init(){
            VCP_ResetPort();    // Подтянули ножку d+ к нулю для правильной идентификации
            VCP_Init();         // Инициализация VCP
        }

        // Отправка по COM порту пакета данных
        void SendPackage(STM_Packages::BasePackage& package){
            CDC_Send_DATA(package.data_ptr, package.len);
        }
    };

} // namespace STM_CppLib

#endif /*   __COM_PORT_HPP   */