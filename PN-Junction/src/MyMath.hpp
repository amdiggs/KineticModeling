//
//  MyMath.hpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/10/24.
//

#ifndef MyMath_hpp
#define MyMath_hpp

#include <stdio.h>

int Clamp(int low,int high, int val);
int Min(int A, int B);
int Max(int A, int B);

float Clamp(float low, float high, float val);
float Min(float A, float B);
float Max(float A, float B);



struct Vec2{
    float x,y;
    Vec2(float x, float y);
    Vec2();
    ~Vec2();
    
    
    Vec2 add(const Vec2& other) const;
    float& operator[](const int index);
    float* get();
    
    float dot(const Vec2& other) const;
    float len() const;
    
    
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2& operator=(const Vec2& other);
    Vec2 operator/(float div);
    Vec2 operator+=(const Vec2& other);
    Vec2 operator*=(float scale);
    void print();
    
    
    
    
};

Vec2 operator*(const Vec2& vec, float scale);
Vec2 operator*(float scale, const Vec2& vec);



struct Vec3{
    float x,y,z;
    Vec3();
    Vec3(float s);
    Vec3(float e_x, float e_y, float e_z);
    
    Vec3 add(const Vec3& other) const;
    float& operator[](const int index);
    float* get();
    
    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
    float len() const;
    
    
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3& operator=(const Vec3& other);
    Vec3 operator/(float div) const;
    Vec3 operator+=(const Vec3& other);
    Vec3 operator-=(const Vec3& other);
    Vec3 operator*=(float scale);
    void Rotate_Quaternion(const Vec3& axis, float ang);
    void Reset();
    void print();
    void Normalize();
    void Vround(int decimals);
    
    
};

Vec3 operator*(float scale, const Vec3& other);
Vec3 operator*(const Vec3& other, float scale);


struct Vec4{
    float r,g,b,a;
    Vec4();
    Vec4(float s);
    Vec4(float e_r, float e_g, float e_b, float e_a);
    Vec4(float* e_vec);
    Vec4(Vec3 vec);
    
    
    float* get();
    float& operator[](const int& index);
    Vec4 operator+(const Vec4& other) const;
    Vec4 operator-(const Vec4& other) const;
    void operator+=(const Vec4& other);
    void operator-=(const Vec4& other);
    void operator*=(float scale);
    void operator/=(float div);
    void Reset();
    
};

Vec4 operator*(float scale, const Vec4& other);
Vec4 operator*(const Vec4& other, float scale);

Vec4 operator/(float scale, const Vec4& other);
Vec4 operator/(const Vec4& other, float scale);


#endif /* MyMath_hpp */
