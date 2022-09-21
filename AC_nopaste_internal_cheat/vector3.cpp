#include "vector3.h"
vector3d::vector3d(const vector3d& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
}

//addition

vector3d vector3d ::operator+(const vector3d& vec)
{
    return vector3d(x + vec.x, y + vec.y, z + vec.z);
}

vector3d& vector3d ::operator+=(const vector3d& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}
//substraction//
vector3d vector3d ::operator-(const vector3d& vec)
{
    return vector3d(x - vec.x, y - vec.y, z - vec.z);
}

vector3d& vector3d::operator-=(const vector3d& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

//scalar multiplication

vector3d vector3d ::operator*(f value)
{
    return vector3d(x * value, y * value, z * value);
}



vector3d& vector3d::operator*=(f value)
{
    x *= value;
    y *= value;
    z *= value;
    return *this;
}

//scalar division
vector3d vector3d ::operator/(f value)
{
    assert(value != 0);
    return vector3d(x / value, y / value, z / value);
}

vector3d& vector3d ::operator/=(f value)
{
    assert(value != 0);
    x /= value;
    y /= value;
    z /= value;
    return *this;
}


vector3d& vector3d::operator=(const vector3d& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}

//Dot product
f vector3d::dot_product(const vector3d& vec)
{
    return x * vec.x + vec.y * y + vec.z * z;
}

//cross product
vector3d vector3d::cross_product(const vector3d& vec)
{
    f ni = y * vec.z - z * vec.y;
    f nj = z * vec.x - x * vec.z;
    f nk = x * vec.y - y * vec.x;
    return vector3d(ni, nj, nk);
}

f vector3d::magnitude()
{
    return sqrt(square());
}

f vector3d::square()
{
    return x * x + y * y + z * z;
}

vector3d vector3d::normalization()
{
    assert(magnitude() != 0);
    *this /= magnitude();
    return *this;
}

f vector3d::distance(const vector3d& vec)
{
    vector3d dist = *this - vec;
    return dist.magnitude();
}

f vector3d::show_X()
{
    return x;
}

f vector3d::show_Y()
{
    return y;
}

f vector3d::show_Z()
{
    return z;
}

void vector3d::disp()
{
    cout << x << " " << y << " " << z << endl;
}

f vector3d::angleBetween(vector3d& vec)
{
    float dot = dot_product(vec);
    return acos(dot / (abs(magnitude()) * abs(vec.magnitude()))) * (180/3.14159265358979);
}