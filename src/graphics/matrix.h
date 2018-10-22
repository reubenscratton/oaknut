// The code in this file is used with the explicit permission of the author.
// All I've done is rename types to fit Oaknut's naming conventions and
// remove some stuff that doesn't apply to graphics.

///////////////////////////////////////////////////////////////////////////////
// MATRIX4
//
// The elements of the matrix are stored as column major order.
//     |  0  4  8 12 |
//     |  1  5  9 13 |
//     |  2  6 10 14 |
//     |  3  7 11 15 |
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2005-06-24
// UPDATED: 2013-09-30
//
// Copyright (C) 2005 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////


class MATRIX4 {
public:
    // constructors
    MATRIX4();  // init with identity
    MATRIX4(const float src[16]);
    MATRIX4(float m00, float m01, float m02, float m03, // 1st column
            float m04, float m05, float m06, float m07, // 2nd column
            float m08, float m09, float m10, float m11, // 3rd column
            float m12, float m13, float m14, float m15);// 4th column

    void        set(const float src[16]);
    void        set(float m00, float m01, float m02, float m03, // 1st column
                    float m04, float m05, float m06, float m07, // 2nd column
                    float m08, float m09, float m10, float m11, // 3rd column
                    float m12, float m13, float m14, float m15);// 4th column
    void        setRow(int index, const float row[4]);
    void        setRow(int index, const VECTOR4& v);
    void        setRow(int index, const VECTOR3& v);
    void        setColumn(int index, const float col[4]);
    void        setColumn(int index, const VECTOR4& v);
    void        setColumn(int index, const VECTOR3& v);

    const float* get() const;
    const float* getTranspose();                        // return transposed matrix
    float        getDeterminant();

    MATRIX4&    identity();
    MATRIX4&    transpose();                            // transpose itself and return reference
    MATRIX4&    invert();                               // check best inverse method before inverse
    MATRIX4&    invertEuclidean();                      // inverse of Euclidean transform matrix
    MATRIX4&    invertGeneral();                        // inverse of generic matrix

    // transform matrix
    MATRIX4&    translate(float x, float y, float z);   // translation by (x,y,z)
    MATRIX4&    translate(const VECTOR3& v);            //
    MATRIX4&    rotate(float angle, const VECTOR3& axis); // rotate angle(degree) along the given axix
    MATRIX4&    rotate(float angle, float x, float y, float z);
    MATRIX4&    rotateX(float angle);                   // rotate on X-axis with degree
    MATRIX4&    rotateY(float angle);                   // rotate on Y-axis with degree
    MATRIX4&    rotateZ(float angle);                   // rotate on Z-axis with degree
    MATRIX4&    scale(float scale);                     // uniform scale
    MATRIX4&    scale(float sx, float sy, float sz);    // scale by (sx, sy, sz) on each axis

    // operators
    MATRIX4     operator+(const MATRIX4& rhs) const;    // add rhs
    MATRIX4     operator-(const MATRIX4& rhs) const;    // subtract rhs
    MATRIX4&    operator+=(const MATRIX4& rhs);         // add rhs and update this object
    MATRIX4&    operator-=(const MATRIX4& rhs);         // subtract rhs and update this object
    VECTOR4     operator*(const VECTOR4& rhs) const;    // multiplication: v' = M * v
    VECTOR3     operator*(const VECTOR3& rhs) const;    // multiplication: v' = M * v
    MATRIX4     operator*(const MATRIX4& rhs) const;    // multiplication: M3 = M1 * M2
    MATRIX4&    operator*=(const MATRIX4& rhs);         // multiplication: M1' = M1 * M2
    bool        operator==(const MATRIX4& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const MATRIX4& rhs) const;   // exact compare, no epsilon
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend MATRIX4 operator-(const MATRIX4& m);                     // unary operator (-)
    friend MATRIX4 operator*(float scalar, const MATRIX4& m);       // pre-multiplication
    friend VECTOR3 operator*(const VECTOR3& vec, const MATRIX4& m); // pre-multiplication
    friend VECTOR4 operator*(const VECTOR4& vec, const MATRIX4& m); // pre-multiplication
    friend std::ostream& operator<<(std::ostream& os, const MATRIX4& m);

    int hash() const;
    
protected:

private:
    float       getCofactor(float m0, float m1, float m2,
                            float m3, float m4, float m5,
                            float m6, float m7, float m8);

    float m[16];
    float tm[16];                                       // transpose m

};





///////////////////////////////////////////////////////////////////////////
// inline functions for MATRIX4
///////////////////////////////////////////////////////////////////////////
inline MATRIX4::MATRIX4()
{
    // initially identity matrix
    identity();
}



inline MATRIX4::MATRIX4(const float src[16])
{
    set(src);
}



inline MATRIX4::MATRIX4(float m00, float m01, float m02, float m03,
                        float m04, float m05, float m06, float m07,
                        float m08, float m09, float m10, float m11,
                        float m12, float m13, float m14, float m15)
{
    set(m00, m01, m02, m03,  m04, m05, m06, m07,  m08, m09, m10, m11,  m12, m13, m14, m15);
}



inline void MATRIX4::set(const float src[16])
{
    m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
    m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
    m[8] = src[8];  m[9] = src[9];  m[10]= src[10]; m[11]= src[11];
    m[12]= src[12]; m[13]= src[13]; m[14]= src[14]; m[15]= src[15];
}



inline void MATRIX4::set(float m00, float m01, float m02, float m03,
                         float m04, float m05, float m06, float m07,
                         float m08, float m09, float m10, float m11,
                         float m12, float m13, float m14, float m15)
{
    m[0] = m00;  m[1] = m01;  m[2] = m02;  m[3] = m03;
    m[4] = m04;  m[5] = m05;  m[6] = m06;  m[7] = m07;
    m[8] = m08;  m[9] = m09;  m[10]= m10;  m[11]= m11;
    m[12]= m12;  m[13]= m13;  m[14]= m14;  m[15]= m15;
}



inline void MATRIX4::setRow(int index, const float row[4])
{
    m[index] = row[0];  m[index + 4] = row[1];  m[index + 8] = row[2];  m[index + 12] = row[3];
}



inline void MATRIX4::setRow(int index, const VECTOR4& v)
{
    m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;  m[index + 12] = v.w;
}



inline void MATRIX4::setRow(int index, const VECTOR3& v)
{
    m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;
}



inline void MATRIX4::setColumn(int index, const float col[4])
{
    m[index*4] = col[0];  m[index*4 + 1] = col[1];  m[index*4 + 2] = col[2];  m[index*4 + 3] = col[3];
}



inline void MATRIX4::setColumn(int index, const VECTOR4& v)
{
    m[index*4] = v.x;  m[index*4 + 1] = v.y;  m[index*4 + 2] = v.z;  m[index*4 + 3] = v.w;
}



inline void MATRIX4::setColumn(int index, const VECTOR3& v)
{
    m[index*4] = v.x;  m[index*4 + 1] = v.y;  m[index*4 + 2] = v.z;
}



inline const float* MATRIX4::get() const
{
    return m;
}



inline const float* MATRIX4::getTranspose()
{
    tm[0] = m[0];   tm[1] = m[4];   tm[2] = m[8];   tm[3] = m[12];
    tm[4] = m[1];   tm[5] = m[5];   tm[6] = m[9];   tm[7] = m[13];
    tm[8] = m[2];   tm[9] = m[6];   tm[10]= m[10];  tm[11]= m[14];
    tm[12]= m[3];   tm[13]= m[7];   tm[14]= m[11];  tm[15]= m[15];
    return tm;
}



inline MATRIX4& MATRIX4::identity()
{
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
    return *this;
}



inline MATRIX4 MATRIX4::operator+(const MATRIX4& rhs) const
{
    return MATRIX4(m[0]+rhs[0],   m[1]+rhs[1],   m[2]+rhs[2],   m[3]+rhs[3],
                   m[4]+rhs[4],   m[5]+rhs[5],   m[6]+rhs[6],   m[7]+rhs[7],
                   m[8]+rhs[8],   m[9]+rhs[9],   m[10]+rhs[10], m[11]+rhs[11],
                   m[12]+rhs[12], m[13]+rhs[13], m[14]+rhs[14], m[15]+rhs[15]);
}



inline MATRIX4 MATRIX4::operator-(const MATRIX4& rhs) const
{
    return MATRIX4(m[0]-rhs[0],   m[1]-rhs[1],   m[2]-rhs[2],   m[3]-rhs[3],
                   m[4]-rhs[4],   m[5]-rhs[5],   m[6]-rhs[6],   m[7]-rhs[7],
                   m[8]-rhs[8],   m[9]-rhs[9],   m[10]-rhs[10], m[11]-rhs[11],
                   m[12]-rhs[12], m[13]-rhs[13], m[14]-rhs[14], m[15]-rhs[15]);
}



inline MATRIX4& MATRIX4::operator+=(const MATRIX4& rhs)
{
    m[0] += rhs[0];   m[1] += rhs[1];   m[2] += rhs[2];   m[3] += rhs[3];
    m[4] += rhs[4];   m[5] += rhs[5];   m[6] += rhs[6];   m[7] += rhs[7];
    m[8] += rhs[8];   m[9] += rhs[9];   m[10]+= rhs[10];  m[11]+= rhs[11];
    m[12]+= rhs[12];  m[13]+= rhs[13];  m[14]+= rhs[14];  m[15]+= rhs[15];
    return *this;
}



inline MATRIX4& MATRIX4::operator-=(const MATRIX4& rhs)
{
    m[0] -= rhs[0];   m[1] -= rhs[1];   m[2] -= rhs[2];   m[3] -= rhs[3];
    m[4] -= rhs[4];   m[5] -= rhs[5];   m[6] -= rhs[6];   m[7] -= rhs[7];
    m[8] -= rhs[8];   m[9] -= rhs[9];   m[10]-= rhs[10];  m[11]-= rhs[11];
    m[12]-= rhs[12];  m[13]-= rhs[13];  m[14]-= rhs[14];  m[15]-= rhs[15];
    return *this;
}



inline VECTOR4 MATRIX4::operator*(const VECTOR4& rhs) const
{
    return VECTOR4(m[0]*rhs.x + m[4]*rhs.y + m[8]*rhs.z  + m[12]*rhs.w,
                   m[1]*rhs.x + m[5]*rhs.y + m[9]*rhs.z  + m[13]*rhs.w,
                   m[2]*rhs.x + m[6]*rhs.y + m[10]*rhs.z + m[14]*rhs.w,
                   m[3]*rhs.x + m[7]*rhs.y + m[11]*rhs.z + m[15]*rhs.w);
}



inline VECTOR3 MATRIX4::operator*(const VECTOR3& rhs) const
{
    return VECTOR3(m[0]*rhs.x + m[4]*rhs.y + m[8]*rhs.z,
                   m[1]*rhs.x + m[5]*rhs.y + m[9]*rhs.z,
                   m[2]*rhs.x + m[6]*rhs.y + m[10]*rhs.z);
}



inline MATRIX4 MATRIX4::operator*(const MATRIX4& n) const
{
    return MATRIX4(m[0]*n[0]  + m[4]*n[1]  + m[8]*n[2]  + m[12]*n[3],   m[1]*n[0]  + m[5]*n[1]  + m[9]*n[2]  + m[13]*n[3],   m[2]*n[0]  + m[6]*n[1]  + m[10]*n[2]  + m[14]*n[3],   m[3]*n[0]  + m[7]*n[1]  + m[11]*n[2]  + m[15]*n[3],
                   m[0]*n[4]  + m[4]*n[5]  + m[8]*n[6]  + m[12]*n[7],   m[1]*n[4]  + m[5]*n[5]  + m[9]*n[6]  + m[13]*n[7],   m[2]*n[4]  + m[6]*n[5]  + m[10]*n[6]  + m[14]*n[7],   m[3]*n[4]  + m[7]*n[5]  + m[11]*n[6]  + m[15]*n[7],
                   m[0]*n[8]  + m[4]*n[9]  + m[8]*n[10] + m[12]*n[11],  m[1]*n[8]  + m[5]*n[9]  + m[9]*n[10] + m[13]*n[11],  m[2]*n[8]  + m[6]*n[9]  + m[10]*n[10] + m[14]*n[11],  m[3]*n[8]  + m[7]*n[9]  + m[11]*n[10] + m[15]*n[11],
                   m[0]*n[12] + m[4]*n[13] + m[8]*n[14] + m[12]*n[15],  m[1]*n[12] + m[5]*n[13] + m[9]*n[14] + m[13]*n[15],  m[2]*n[12] + m[6]*n[13] + m[10]*n[14] + m[14]*n[15],  m[3]*n[12] + m[7]*n[13] + m[11]*n[14] + m[15]*n[15]);
}



inline MATRIX4& MATRIX4::operator*=(const MATRIX4& rhs)
{
    *this = *this * rhs;
    return *this;
}



inline bool MATRIX4::operator==(const MATRIX4& n) const
{
    return (m[0] == n[0])  && (m[1] == n[1])  && (m[2] == n[2])  && (m[3] == n[3])  &&
           (m[4] == n[4])  && (m[5] == n[5])  && (m[6] == n[6])  && (m[7] == n[7])  &&
           (m[8] == n[8])  && (m[9] == n[9])  && (m[10]== n[10]) && (m[11]== n[11]) &&
           (m[12]== n[12]) && (m[13]== n[13]) && (m[14]== n[14]) && (m[15]== n[15]);
}



inline bool MATRIX4::operator!=(const MATRIX4& n) const
{
    return (m[0] != n[0])  || (m[1] != n[1])  || (m[2] != n[2])  || (m[3] != n[3])  ||
           (m[4] != n[4])  || (m[5] != n[5])  || (m[6] != n[6])  || (m[7] != n[7])  ||
           (m[8] != n[8])  || (m[9] != n[9])  || (m[10]!= n[10]) || (m[11]!= n[11]) ||
           (m[12]!= n[12]) || (m[13]!= n[13]) || (m[14]!= n[14]) || (m[15]!= n[15]);
}



inline float MATRIX4::operator[](int index) const
{
    return m[index];
}



inline float& MATRIX4::operator[](int index)
{
    return m[index];
}



inline MATRIX4 operator-(const MATRIX4& rhs)
{
    return MATRIX4(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
}



inline MATRIX4 operator*(float s, const MATRIX4& rhs)
{
    return MATRIX4(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
}



inline VECTOR4 operator*(const VECTOR4& v, const MATRIX4& m)
{
    return VECTOR4(v.x*m[0] + v.y*m[1] + v.z*m[2] + v.w*m[3],  v.x*m[4] + v.y*m[5] + v.z*m[6] + v.w*m[7],  v.x*m[8] + v.y*m[9] + v.z*m[10] + v.w*m[11], v.x*m[12] + v.y*m[13] + v.z*m[14] + v.w*m[15]);
}



inline VECTOR3 operator*(const VECTOR3& v, const MATRIX4& m)
{
    return VECTOR3(v.x*m[0] + v.y*m[1] + v.z*m[2],  v.x*m[4] + v.y*m[5] + v.z*m[6],  v.x*m[8] + v.y*m[9] + v.z*m[10]);
}


