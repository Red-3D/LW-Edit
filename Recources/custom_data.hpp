#pragma once
#define _USE_MATH_DEFINES
#ifndef _CMATH_
#include<cmath>
#endif

#ifndef _LW_Edit_Custom_Data_
#define _LW_Edit_Custom_Data_

struct Quaternion {
    double w, x, y, z;
};

struct Euler_Position {
    float x, y, z;
};

struct Euler_Rotation {
    float x, y, z;
};

Quaternion ToQuaternion(double X, double Y, double Z);
Euler_Rotation ToEulerAngles(Quaternion Q);
#endif