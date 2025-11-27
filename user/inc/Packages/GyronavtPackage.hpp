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
#define RegAdr              0x00
#define GyronavtPackageLen  53

#define BarConst            3.14159f
#define DefaultStatus       0x32

#define TimeBaseIndex       4
#define StatusIndex         8
#define AccBaseIndex        12
#define GyroBaseIndex       24
#define MagBaseIndex        36
#define BarBaseIndex        48
#define ControlSumIndex     52

/* Global variables ----------------------------------------------------------*/
extern STM_CppLib::L3GD20       gyro_sensor;
extern STM_CppLib::LSM303DLHC   acc_sensor;
extern uint32_t                 tick_counter;

// -----------------------------------------------------------------------------

namespace STM_CppLib{
    namespace STM_Packages{

    // Посылка, согласно протоколу Гиронавт
    class GyronavtPackage: public BasePackage{
        uint8_t data_arr[GyronavtPackageLen] = {0};

        uint32_t time;
        uint8_t status;
        TriaxialData acc_data, gyro_data, mag_data;
        float bar;

        // Переменные для сохранения float данных с датчиков в uint8_t* data_arr
        FloatBytes float_bytes;
        const uint8_t* float_bytes_arr = float_bytes.get_bytes();

        // Сохранение трёхосевых данных в массив data_arr по базовому индексу
        void insert_triaxial_data(TriaxialData triaxial_data, uint8_t base_index){
            insert_float(triaxial_data.x_coord, base_index + 0);
            insert_float(triaxial_data.y_coord, base_index + 4);
            insert_float(triaxial_data.z_coord, base_index + 8);
        }

        // Сохранение в массив data_arr числа во float по базовому индексу
        void insert_float(float value, uint8_t base_index){
            float_bytes = value;

            data_arr[base_index + 0] = float_bytes_arr[0];
            data_arr[base_index + 1] = float_bytes_arr[1];
            data_arr[base_index + 2] = float_bytes_arr[2];
            data_arr[base_index + 3] = float_bytes_arr[3];
        }

    public:
        GyronavtPackage(): 
            time(0), 
            status(DefaultStatus), 
            bar(BarConst){
                len = GyronavtPackageLen;
                data_ptr = data_arr;

                data_arr[0] = Preamble;
                data_arr[1] = DevAdr;
                data_arr[2] = RegAdr;
                data_arr[3] = GyronavtPackageLen;
        }
        
        void UpdateData() {
            time = tick_counter;

            acc_data = acc_sensor.acc_data;
            gyro_data = gyro_sensor.gyro_data;
            mag_data = acc_sensor.mag_data;

            DataPackaging();
        }

        void UpdateTime(uint32_t new_time){
            time = new_time;
            
            data_arr[TimeBaseIndex + 0] = static_cast<uint8_t>(time);
            data_arr[TimeBaseIndex + 1] = static_cast<uint8_t>(time >> 8);
            data_arr[TimeBaseIndex + 2] = static_cast<uint8_t>(time >> 16);
            data_arr[TimeBaseIndex + 3] = static_cast<uint8_t>(time >> 24);
        }
        
    private:

        // Вычисление контрольной суммы согласно документации
        uint8_t CountControlSum(){
            uint8_t crc = 0xFF;

            for (uint8_t i = 0; i < len; i++){
                crc ^= data_arr[i];

                for(uint8_t j = 0; j < 8; j++)
                    crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
            }
            return crc;
        }

        void DataPackaging()  {
            
            // -------------------------------------
            // Заполним данные времени
            data_arr[TimeBaseIndex + 0] = static_cast<uint8_t>(time);           // 1 разряд
            data_arr[TimeBaseIndex + 1] = static_cast<uint8_t>(time >> 8);      // 2 разряд
            data_arr[TimeBaseIndex + 2] = static_cast<uint8_t>(time >> 16);     // 3 разряд
            data_arr[TimeBaseIndex + 3] = static_cast<uint8_t>(time >> 24);     // 4 разряд
            
            // -------------------------------------
            // Заполним статус
            data_arr[StatusIndex] = status;

            // -------------------------------------
            // Заполним данные с датчиков
            insert_triaxial_data(acc_data,  AccBaseIndex);
            insert_triaxial_data(gyro_data, GyroBaseIndex);
            insert_triaxial_data(mag_data,  MagBaseIndex);

            // -------------------------------------
            // Заполним данные барометра
            insert_float(bar, BarBaseIndex);

            // -------------------------------------
            // Заполним контрольную сумму
            data_arr[ControlSumIndex] = CountControlSum();        
        }
    };

    } // namespace STM_Packages
} // namespace STM_CppLib

#endif /*   __GYRONAVT_PACKAGE_HPP   */