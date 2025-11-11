/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATA_FRAME_HPP
#define __DATA_FRAME_HPP

/* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
// Класс для работы с данными, имеющие три координаты
class TriaxialData{

public:
    float x_coord;
    float y_coord;
    float z_coord;

    // ------------------------------
    // Конструкторы и деструктор
    // ------------------------------
    TriaxialData(): x_coord(0), y_coord(0), z_coord(0) {}
    TriaxialData(float _x, float _y, float _z): x_coord(_x), y_coord(_y), z_coord(_z) {}
    TriaxialData(TriaxialData& triaxial_data): x_coord(triaxial_data.x_coord), y_coord(triaxial_data.y_coord), z_coord(triaxial_data.z_coord) {}

    ~TriaxialData() {}

    // ------------------------------
    // Перегрузка операторов
    // ------------------------------
    float& operator[](int index){
        if      (index == 0) return X_coord;
        else if (index == 1) return Y_coord;
        else if (index == 2) return Z_coord;
    }

    TriaxialData operator+(const TriaxialData& other) const {
        return TriaxialData(x + other.x, y + other.y, z + other.z);
    }

    TriaxialData operator-(const TriaxialData& other) const {
        return TriaxialData(x - other.x, y - other.y, z - other.z);
    }

    TriaxialData operator*(double scalar) const {
        return TriaxialData(x * scalar, y * scalar, z * scalar);
    }

    TriaxialData operator/(double scalar) const {
        return TriaxialData(x / scalar, y / scalar, z / scalar);
    }

    // 2. Составные присваивания
    TriaxialData& operator+=(const TriaxialData& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    TriaxialData& operator-=(const TriaxialData& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    TriaxialData& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    TriaxialData& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
}


#endif /*   __DATA_FRAME_HPP   */