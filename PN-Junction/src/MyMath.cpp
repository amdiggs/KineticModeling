//
//  MyMath.cpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/10/24.
//

#include "MyMath.hpp"
#include <math.h>


float Round(float f, int dig)
{
    float scale = 10.0*dig;
    int x = round(f*scale);
    return x / scale;
}

int Clamp(int low, int high, int val)
{
    int tmp =  (val > low) ? val : low;
    return (tmp < high) ? tmp : high;
}
int Min(int A, int B)
{
    return (A < B) ? A : B;
}
int Max(int A, int B)
{
    return (A > B) ? A : B;
}

float Clamp(float low, float high, float val)
{
    float tmp = (val > low) ? val : low;
    return (tmp < high) ? tmp : high;
}
float Min(float A, float B)
{
    return (A < B) ? A : B;
}

float Max(float A, float B)
{
    return (A > B) ? A : B;
}

//####################################################################################
//####################################################################################
//####################################################################################
Vec2::Vec2()
:x(0.0), y(0.0)
{}

Vec2::Vec2(float x, float y)
:x(x), y(y)
{}
Vec2::~Vec2() {}

Vec2 Vec2::add(const Vec2 &other) const {
    return Vec2(x + other.x, y+other.y);
}

float *Vec2::get() {
    return &x;;
}

float &Vec2::operator[](const int index) {
    return get()[index];
}


float Vec2::dot(const Vec2 &other) const {
    return x*other.x + y*other.y;
}

float Vec2::len() const {
    return sqrt(this->dot(*this));
}


Vec2 Vec2::operator+(const Vec2 &other) const {
    return add(other);
}

Vec2 Vec2::operator-(const Vec2 &other) const {
    float _x = this->x - other.x;
    float _y = this->y - other.y;
    
    return Vec2(_x,_y);
}

Vec2 &Vec2::operator=(const Vec2 &other) {
    if (this == &other) {
        return *this;
    }
    
    this->x = other.x;
    this->y = other.y;
    return *this;
}


Vec2 Vec2::operator/(float div) {
    return Vec2(this->x/div, this->y/div);
}

Vec2 Vec2::operator+=(const Vec2 &other) {
        this->x = this->x+ other.x;
        this->y = this->y + other.y;
        
        return *this;
}

Vec2 Vec2::operator*=(float scale) {
        this->x = this->x*scale;
        this->y = this->y*scale;
        
        return *this;
}

void Vec2::print() {
    printf("(x,y) = (%.2f,%.2f)\n",this->x,this->y);
}


Vec2 operator*(const Vec2& vec, float scale) {
    float _x = vec.x*scale;
    float _y = vec.y*scale;
    
    return Vec2(_x,_y);
}

Vec2 operator*(float scale,const Vec2& vec) {
    float _x = vec.x*scale;
    float _y = vec.y*scale;
    
    return Vec2(_x,_y);
}


//###################################################################
//###################################################################
//###################################################################

Vec3::Vec3()
:x(0.0), y(0.0), z(0.0)
{}


Vec3::Vec3(float s)
:x(s) , y(s) , z(s)
{}

Vec3::Vec3(float e_x, float e_y, float e_z)
:x(e_x), y(e_y),z(e_z)
{}


float* Vec3::get(){
    return &x;
}


float& Vec3::operator[](const int index){
    return get()[index];
}

float Vec3::dot(const Vec3 &other) const {
    return x*other.x + y*other.y + z*other.z;
}

Vec3 Vec3::cross(const Vec3 &other) const {
    float _x = y*other.z - z*other.y;
    float _y = z*other.x - x*other.z;
    float _z = x*other.y - y*other.x;
    return Vec3(_x, _y, _z);
}

float Vec3::len() const{
    return sqrt(this->dot(*(this)));
}

Vec3 &Vec3::operator=(const Vec3 &other) {
    if (&other == this){
        return  *this;
    }
    else {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }
    return *this;
}




Vec3 Vec3::operator/(float div) const{
    return Vec3(x/div,y/div,z/div);
}


Vec3 Vec3::operator-(const Vec3& other) const{
    float _x = this->x - other.x;
    float _y = this->y - other.y;
    float _z = this->z - other.z;
    return Vec3(_x,_y,_z);
    
}




Vec3 Vec3::operator+(const Vec3& other) const{
    float _x = this->x + other.x;
    float _y = this->y + other.y;
    float _z = this->z + other.z;
    return Vec3(_x,_y,_z);
    
}




Vec3 Vec3::operator+=(const Vec3& other){
    this->x = this->x + other.x;
    this->y = this->y + other.y;
    this->z = this->z + other.z;
    
    return *this;
}

Vec3 Vec3::operator-=(const Vec3& other){
    this->x = this->x - other.x;
    this->y = this->y - other.y;
    this->z = this->z - other.z;
    
    return *this;
}

Vec3 Vec3::operator*=(float scale){
    *this = *this * scale;
    return *this;
    
}

void Vec3::print(){
    printf("x = %.2f, y = %.2f, z = %.2f\n",x,y,z);
}


void Vec3::Reset(){
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

void Vec3::Normalize(){
    float N = this->len();
    *this = (*this)/N;
}




void Vec3::Vround(int decimals){
    x = Round(x, decimals);
    y = Round(y, decimals);
    z = Round(z, decimals);
}


Vec3 operator*(float scale, const Vec3& other){
    return Vec3(other.x*scale,other.y*scale, other.z*scale);
}

Vec3 operator*(const Vec3& other, float scale){
    return Vec3(other.x*scale,other.y*scale, other.z*scale);
}



//========================================================================================================

Vec4::Vec4()
:r(0.0) , g(0.0) , b(0.0) , a(0.0)
{}


Vec4::Vec4(float s)
:r(s) , g(s) , b(s) , a(s)
{}

Vec4::Vec4(float e_r, float e_g, float e_b, float e_a)
:r(e_r) , g(e_g) , b(e_b) , a(e_a)
{}

Vec4::Vec4(float* e_vec)
:r(e_vec[0]), g(e_vec[1]), b(e_vec[2]), a(e_vec[3]){}

Vec4::Vec4(Vec3 vec)
:r(vec[0]), g(vec[1]), b(vec[2]), a(1.0){}

float* Vec4::get(){
    return &r;
}


float& Vec4::operator[](const int& index){
    return get()[index];
}

Vec4 Vec4::operator+(const Vec4& other) const{
    
    float _x = this->r + other.r;
    float _y = this->g + other.g;
    float _z = this->b + other.b;
    float _w = this->a + other.a;
    return Vec4(_x,_y,_z, _w);
    }

Vec4 Vec4::operator-(const Vec4& other) const{
    
    float _x = this->r - other.r;
    float _y = this->g - other.g;
    float _z = this->b - other.b;
    float _w = this->a - other.a;
    return Vec4(_x,_y,_z, _w);
    }
void Vec4::operator+=(const Vec4& other){
    
    this->r += other.r;
    this->g += other.g;
    this->b += other.b;
    this->a += other.a;
    }
void Vec4::operator-=(const Vec4& other){
    
    this->r -= other.r;
    this->g -= other.g;
    this->b -= other.b;
    this->a -= other.a;
    }

void Vec4::operator*=(float scale){
    
    this->r *= scale;
    this->g *= scale;
    this->b *= scale;
    this->a *= scale;
    }

void Vec4::operator/=(float div){
    (*this)*= 1.0/div;
}


void Vec4::Reset(){
    r = 0.0;
    g = 0.0;
    b = 0.0;
    a = 0.0;
}


Vec4 operator*(float scale, const Vec4& other){
    return Vec4(other.r*scale,other.g*scale, other.b*scale, other.a*scale);
}

Vec4 operator*(const Vec4& other, float scale){
    return Vec4(other.r*scale,other.g*scale, other.b*scale, other.a*scale);
}



Vec4 operator/(const Vec4& other, float scale){
    return Vec4(other.r/scale,other.g/scale, other.b/scale, other.a/scale);
}
