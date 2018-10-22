// The code in this file is used with the explicit permission of the author.
// All I've done is rename types to fit Oaknut's naming conventions and
// remove some stuff that doesn't apply to graphics.

///////////////////////////////////////////////////////////////////////////////
// Vectors.h
// =========
// 2D/3D/4D vectors
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2007-02-14
// UPDATED: 2013-01-20
//
// Copyright (C) 2007-2013 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct VECTOR2
{
    float x;
    float y;

    // ctors
    VECTOR2() : x(0), y(0) {};
    VECTOR2(float x, float y) : x(x), y(y) {};

    // utils functions
    void        set(float x, float y);
    float       length() const;                         //
    float       distance(const VECTOR2& vec) const;     // distance between two vectors
    VECTOR2&    normalize();                            //
    float       dot(const VECTOR2& vec) const;          // dot product
    bool        equal(const VECTOR2& vec, float e) const; // compare with epsilon

    // operators
    VECTOR2     operator-() const;                      // unary operator (negate)
    VECTOR2     operator+(const VECTOR2& rhs) const;    // add rhs
    VECTOR2     operator-(const VECTOR2& rhs) const;    // subtract rhs
    VECTOR2&    operator+=(const VECTOR2& rhs);         // add rhs and update this object
    VECTOR2&    operator-=(const VECTOR2& rhs);         // subtract rhs and update this object
    VECTOR2     operator*(const float scale) const;     // scale
    VECTOR2     operator*(const VECTOR2& rhs) const;    // multiply each element
    VECTOR2&    operator*=(const float scale);          // scale and update this object
    VECTOR2&    operator*=(const VECTOR2& rhs);         // multiply each element and update this object
    VECTOR2     operator/(const float scale) const;     // inverse scale
    VECTOR2&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const VECTOR2& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const VECTOR2& rhs) const;   // exact compare, no epsilon
    bool        operator<(const VECTOR2& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend VECTOR2 operator*(const float a, const VECTOR2 vec);
};



///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct VECTOR3
{
    float x;
    float y;
    float z;

    // ctors
    VECTOR3() : x(0), y(0), z(0) {};
    VECTOR3(float x, float y, float z) : x(x), y(y), z(z) {};

    // utils functions
    void        set(float x, float y, float z);
    float       length() const;                         //
    float       distance(const VECTOR3& vec) const;     // distance between two vectors
    VECTOR3&    normalize();                            //
    float       dot(const VECTOR3& vec) const;          // dot product
    VECTOR3     cross(const VECTOR3& vec) const;        // cross product
    bool        equal(const VECTOR3& vec, float e) const; // compare with epsilon

    // operators
    VECTOR3     operator-() const;                      // unary operator (negate)
    VECTOR3     operator+(const VECTOR3& rhs) const;    // add rhs
    VECTOR3     operator-(const VECTOR3& rhs) const;    // subtract rhs
    VECTOR3&    operator+=(const VECTOR3& rhs);         // add rhs and update this object
    VECTOR3&    operator-=(const VECTOR3& rhs);         // subtract rhs and update this object
    VECTOR3     operator*(const float scale) const;     // scale
    VECTOR3     operator*(const VECTOR3& rhs) const;    // multiplay each element
    VECTOR3&    operator*=(const float scale);          // scale and update this object
    VECTOR3&    operator*=(const VECTOR3& rhs);         // product each element and update this object
    VECTOR3     operator/(const float scale) const;     // inverse scale
    VECTOR3&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const VECTOR3& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const VECTOR3& rhs) const;   // exact compare, no epsilon
    bool        operator<(const VECTOR3& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend VECTOR3 operator*(const float a, const VECTOR3 vec);
};



///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct VECTOR4
{
    float x;
    float y;
    float z;
    float w;

    // ctors
    VECTOR4() : x(0), y(0), z(0), w(0) {};
    VECTOR4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};

    // utils functions
    void        set(float x, float y, float z, float w);
    float       length() const;                         //
    float       distance(const VECTOR4& vec) const;     // distance between two vectors
    VECTOR4&    normalize();                            //
    float       dot(const VECTOR4& vec) const;          // dot product
    bool        equal(const VECTOR4& vec, float e) const; // compare with epsilon

    // operators
    VECTOR4     operator-() const;                      // unary operator (negate)
    VECTOR4     operator+(const VECTOR4& rhs) const;    // add rhs
    VECTOR4     operator-(const VECTOR4& rhs) const;    // subtract rhs
    VECTOR4&    operator+=(const VECTOR4& rhs);         // add rhs and update this object
    VECTOR4&    operator-=(const VECTOR4& rhs);         // subtract rhs and update this object
    VECTOR4     operator*(const float scale) const;     // scale
    VECTOR4     operator*(const VECTOR4& rhs) const;    // multiply each element
    VECTOR4&    operator*=(const float scale);          // scale and update this object
    VECTOR4&    operator*=(const VECTOR4& rhs);         // multiply each element and update this object
    VECTOR4     operator/(const float scale) const;     // inverse scale
    VECTOR4&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const VECTOR4& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const VECTOR4& rhs) const;   // exact compare, no epsilon
    bool        operator<(const VECTOR4& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend VECTOR4 operator*(const float a, const VECTOR4 vec);
};



// fast math routines from Doom3 SDK
inline float invSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;          // get bits for floating value
    i = 0x5f3759df - (i>>1);    // gives initial guess
    x = *(float*)&i;            // convert bits back to float
    x = x * (1.5f - xhalf*x*x); // Newton step
    return x;
}



///////////////////////////////////////////////////////////////////////////////
// inline functions for VECTOR2
///////////////////////////////////////////////////////////////////////////////
inline VECTOR2 VECTOR2::operator-() const {
    return VECTOR2(-x, -y);
}

inline VECTOR2 VECTOR2::operator+(const VECTOR2& rhs) const {
    return VECTOR2(x+rhs.x, y+rhs.y);
}

inline VECTOR2 VECTOR2::operator-(const VECTOR2& rhs) const {
    return VECTOR2(x-rhs.x, y-rhs.y);
}

inline VECTOR2& VECTOR2::operator+=(const VECTOR2& rhs) {
    x += rhs.x; y += rhs.y; return *this;
}

inline VECTOR2& VECTOR2::operator-=(const VECTOR2& rhs) {
    x -= rhs.x; y -= rhs.y; return *this;
}

inline VECTOR2 VECTOR2::operator*(const float a) const {
    return VECTOR2(x*a, y*a);
}

inline VECTOR2 VECTOR2::operator*(const VECTOR2& rhs) const {
    return VECTOR2(x*rhs.x, y*rhs.y);
}

inline VECTOR2& VECTOR2::operator*=(const float a) {
    x *= a; y *= a; return *this;
}

inline VECTOR2& VECTOR2::operator*=(const VECTOR2& rhs) {
    x *= rhs.x; y *= rhs.y; return *this;
}

inline VECTOR2 VECTOR2::operator/(const float a) const {
    return VECTOR2(x/a, y/a);
}

inline VECTOR2& VECTOR2::operator/=(const float a) {
    x /= a; y /= a; return *this;
}

inline bool VECTOR2::operator==(const VECTOR2& rhs) const {
    return (x == rhs.x) && (y == rhs.y);
}

inline bool VECTOR2::operator!=(const VECTOR2& rhs) const {
    return (x != rhs.x) || (y != rhs.y);
}

inline bool VECTOR2::operator<(const VECTOR2& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    return false;
}

inline float VECTOR2::operator[](int index) const {
    return (&x)[index];
}

inline float& VECTOR2::operator[](int index) {
    return (&x)[index];
}

inline void VECTOR2::set(float x, float y) {
    this->x = x; this->y = y;
}

inline float VECTOR2::length() const {
    return sqrtf(x*x + y*y);
}

inline float VECTOR2::distance(const VECTOR2& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y));
}

inline VECTOR2& VECTOR2::normalize() {
    //@@const float EPSILON = 0.000001f;
    float xxyy = x*x + y*y;
    //@@if(xxyy < EPSILON)
    //@@    return *this;

    //float invLength = invSqrt(xxyy);
    float invLength = 1.0f / sqrtf(xxyy);
    x *= invLength;
    y *= invLength;
    return *this;
}

inline float VECTOR2::dot(const VECTOR2& rhs) const {
    return (x*rhs.x + y*rhs.y);
}

inline bool VECTOR2::equal(const VECTOR2& rhs, float epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
}

inline VECTOR2 operator*(const float a, const VECTOR2 vec) {
    return VECTOR2(a*vec.x, a*vec.y);
}

// END OF VECTOR2 /////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// inline functions for VECTOR3
///////////////////////////////////////////////////////////////////////////////
inline VECTOR3 VECTOR3::operator-() const {
    return VECTOR3(-x, -y, -z);
}

inline VECTOR3 VECTOR3::operator+(const VECTOR3& rhs) const {
    return VECTOR3(x+rhs.x, y+rhs.y, z+rhs.z);
}

inline VECTOR3 VECTOR3::operator-(const VECTOR3& rhs) const {
    return VECTOR3(x-rhs.x, y-rhs.y, z-rhs.z);
}

inline VECTOR3& VECTOR3::operator+=(const VECTOR3& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

inline VECTOR3& VECTOR3::operator-=(const VECTOR3& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

inline VECTOR3 VECTOR3::operator*(const float a) const {
    return VECTOR3(x*a, y*a, z*a);
}

inline VECTOR3 VECTOR3::operator*(const VECTOR3& rhs) const {
    return VECTOR3(x*rhs.x, y*rhs.y, z*rhs.z);
}

inline VECTOR3& VECTOR3::operator*=(const float a) {
    x *= a; y *= a; z *= a; return *this;
}

inline VECTOR3& VECTOR3::operator*=(const VECTOR3& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

inline VECTOR3 VECTOR3::operator/(const float a) const {
    return VECTOR3(x/a, y/a, z/a);
}

inline VECTOR3& VECTOR3::operator/=(const float a) {
    x /= a; y /= a; z /= a; return *this;
}

inline bool VECTOR3::operator==(const VECTOR3& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool VECTOR3::operator!=(const VECTOR3& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

inline bool VECTOR3::operator<(const VECTOR3& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    return false;
}

inline float VECTOR3::operator[](int index) const {
    return (&x)[index];
}

inline float& VECTOR3::operator[](int index) {
    return (&x)[index];
}

inline void VECTOR3::set(float x, float y, float z) {
    this->x = x; this->y = y; this->z = z;
}

inline float VECTOR3::length() const {
    return sqrtf(x*x + y*y + z*z);
}

inline float VECTOR3::distance(const VECTOR3& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z));
}

inline VECTOR3& VECTOR3::normalize() {
    //@@const float EPSILON = 0.000001f;
    float xxyyzz = x*x + y*y + z*z;
    //@@if(xxyyzz < EPSILON)
    //@@    return *this; // do nothing if it is ~zero vector

    //float invLength = invSqrt(xxyyzz);
    float invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline float VECTOR3::dot(const VECTOR3& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

inline VECTOR3 VECTOR3::cross(const VECTOR3& rhs) const {
    return VECTOR3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
}

inline bool VECTOR3::equal(const VECTOR3& rhs, float epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

inline VECTOR3 operator*(const float a, const VECTOR3 vec) {
    return VECTOR3(a*vec.x, a*vec.y, a*vec.z);
}

// END OF VECTOR3 /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for VECTOR4
///////////////////////////////////////////////////////////////////////////////
inline VECTOR4 VECTOR4::operator-() const {
    return VECTOR4(-x, -y, -z, -w);
}

inline VECTOR4 VECTOR4::operator+(const VECTOR4& rhs) const {
    return VECTOR4(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

inline VECTOR4 VECTOR4::operator-(const VECTOR4& rhs) const {
    return VECTOR4(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

inline VECTOR4& VECTOR4::operator+=(const VECTOR4& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

inline VECTOR4& VECTOR4::operator-=(const VECTOR4& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

inline VECTOR4 VECTOR4::operator*(const float a) const {
    return VECTOR4(x*a, y*a, z*a, w*a);
}

inline VECTOR4 VECTOR4::operator*(const VECTOR4& rhs) const {
    return VECTOR4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

inline VECTOR4& VECTOR4::operator*=(const float a) {
    x *= a; y *= a; z *= a; w *= a; return *this;
}

inline VECTOR4& VECTOR4::operator*=(const VECTOR4& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

inline VECTOR4 VECTOR4::operator/(const float a) const {
    return VECTOR4(x/a, y/a, z/a, w/a);
}

inline VECTOR4& VECTOR4::operator/=(const float a) {
    x /= a; y /= a; z /= a; w /= a; return *this;
}

inline bool VECTOR4::operator==(const VECTOR4& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

inline bool VECTOR4::operator!=(const VECTOR4& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

inline bool VECTOR4::operator<(const VECTOR4& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    if(w < rhs.w) return true;
    if(w > rhs.w) return false;
    return false;
}

inline float VECTOR4::operator[](int index) const {
    return (&x)[index];
}

inline float& VECTOR4::operator[](int index) {
    return (&x)[index];
}

inline void VECTOR4::set(float x, float y, float z, float w) {
    this->x = x; this->y = y; this->z = z; this->w = w;
}

inline float VECTOR4::length() const {
    return sqrtf(x*x + y*y + z*z + w*w);
}

inline float VECTOR4::distance(const VECTOR4& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z) + (vec.w-w)*(vec.w-w));
}

inline VECTOR4& VECTOR4::normalize() {
    //NOTE: leave w-component untouched
    //@@const float EPSILON = 0.000001f;
    float xxyyzz = x*x + y*y + z*z;
    //@@if(xxyyzz < EPSILON)
    //@@    return *this; // do nothing if it is zero vector

    //float invLength = invSqrt(xxyyzz);
    float invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline float VECTOR4::dot(const VECTOR4& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w);
}

inline bool VECTOR4::equal(const VECTOR4& rhs, float epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
           fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
}

inline VECTOR4 operator*(const float a, const VECTOR4 vec) {
    return VECTOR4(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}


// END OF VECTOR4 /////////////////////////////////////////////////////////////

