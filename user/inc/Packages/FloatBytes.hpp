/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLOAT_BYTES_HPP
#define __FLOAT_BYTES_HPP

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

// -----------------------------------------------------------------------------

namespace STM_CppLib{
    namespace STM_Packages{

        // Класс для преобразования float в uint8_t[4]
        class FloatBytes {
        private:
            union {
                float value;
                uint8_t bytes[sizeof(float)];
            } data;

        public:
            // Конструктор
            FloatBytes(){
                data.value = 0.0f;
            }

            // Оператор присваивания
            FloatBytes& operator=(float f) {
                data.value = f;
                return *this;
            }

            // Получение указателя на массив байтов 
            const uint8_t* get_bytes() const {
                return data.bytes;
            }
        };

    } // namespace STM_Packages
} // namespace STM_CppLib




#endif /*   __FLOAT_BYTES_HPP   */