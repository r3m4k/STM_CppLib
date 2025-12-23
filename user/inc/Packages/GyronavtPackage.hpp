/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GYRONAVT_PACKAGE_HPP
#define __GYRONAVT_PACKAGE_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "BasePackage.hpp"
#include "TriaxialData.hpp"
#include "FloatBytes.hpp"
#include "L3GD20.hpp"
#include "LSM303DLHC.hpp"

/* Defines -------------------------------------------------------------------*/
#define Preamble            0xFB
#define DevAdr              0xFF
#define RegAdr              0xFF

#define BarConst            3.14159f
#define DefaultStatus       0x32

/* Global variables ----------------------------------------------------------*/
extern STM_CppLib::L3GD20       gyro_sensor;
extern STM_CppLib::LSM303DLHC   acc_sensor;

// -----------------------------------------------------------------------------

namespace STM_CppLib{
    namespace STM_Packages{

    // ------------------------------------    
    // Посылка, согласно протоколу Гиронавт
    class GyronavtPackage: public BasePackage{
    
        #pragma pack(1)
        struct package_body_t
        {
            uint8_t header[4] = {Preamble, DevAdr, RegAdr, 0};
            uint32_t time = 0;
            uint8_t status = DefaultStatus;
            uint8_t hole[3] = {0};
            TriaxialData acc_data, gyro_data, mag_data;            
            float bar = BarConst;
            uint8_t control_sum = 0;
        } package_body;
        #pragma pack()

        static_assert(
            (sizeof(package_body) - sizeof(package_body.header) - sizeof(package_body.control_sum)) == 48,
            "Incorrect length of data inside the Gyronavt package"
        );

    public:
        GyronavtPackage(){
            // Последним байтом заголовка необходимо задать длину данных внутри посылки
            package_body.header[3] = sizeof(package_body) - sizeof(package_body.header);
            
            len = sizeof(package_body);
            data_ptr = reinterpret_cast<uint8_t*>(&package_body);
        }

        void UpdateData() {
            package_body.acc_data = acc_sensor.acc_data;
            package_body.gyro_data = gyro_sensor.gyro_data;
            package_body.mag_data = acc_sensor.mag_data;
        }

        void UpdateTime(uint32_t new_time){
            package_body.time = new_time;
        }

        void UpdateControlSum(){
            package_body.control_sum = CountControlSum();
        }
        
    private:

        // Вычисление контрольной суммы согласно документации
        uint8_t CountControlSum(){
            uint8_t crc = 0xFF;

            // Условие len-1 необходимо, чтобы не учитывать в расчёте контрольной суммы
            // не учитывать старое значение контрольной суммы
            for (uint8_t i = 0; i < len-1; i++){
                crc ^= data_ptr[i];

                for(uint8_t j = 0; j < 8; j++)
                    crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
            }
            return crc;
        }        
    };

    } // namespace STM_Packages
} // namespace STM_CppLib

#endif /*   __GYRONAVT_PACKAGE_HPP   */