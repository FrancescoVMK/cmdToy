
#include <math.h>
#include "cmdtoymath.h"


const float PI = 3.141592653589793;
  //functions
  
  float float_mix(float x, float y, float a) {
      return x * (1.0f - a) + y * a;
  }
  
  // Multiply mat2 with vec2
  vec2 mat2_times_vec2(mat2 a, vec2 b) {
      vec2 result;
      result.x = a.x1 * b.x + a.y1 * b.y;
      result.y = a.x2 * b.x + a.y2 * b.y;
      return result;
  }
  
  // Add two vec2
  vec2 vec2_add(vec2 a, vec2 b) {
      return (vec2){a.x + b.x, a.y + b.y};
  }
  
  //add two vec2 in place 
  void vec2_add_p(vec2 *a, vec2 b) {
      a->x += b.x;
      a->y += b.y;
  }
  
  // Add three vec2
  vec2 vec2_add_3(vec2 a, vec2 b, vec2 c) {
      return (vec2){a.x + b.x + c.x, a.y + b.y + c.y};
  }
  
  // Subtract two vec2
  vec2 vec2_sub(vec2 a, vec2 b) {
      return (vec2){a.x - b.x, a.y - b.y};
  }
  
  void vec2_sub_p(vec2 *a, vec2 b) {
      a->x -= b.x;
      a->y -= b.y;
  }
  
  // Multiply two vec2
  vec2 vec2_mul(vec2 a, vec2 b) {
      return (vec2){a.x * b.x, a.y * b.y};
  }
  
  // Divide two vec2
  vec2 vec2_div(vec2 a, vec2 b) {
      return (vec2){a.x / b.x, a.y / b.y};
  }
  
  // Scale vec2 by scalar
  vec2 vec2_scale(vec2 a, float s) {
      return (vec2){a.x * s, a.y * s};
  }
  
  void vec2_scale_p(vec2 *a, float s) {
      a->x *= s;
      a->y *= s;
  }
  
  // Calculate length of vec2
  float vec2_length(vec2 a) {
      return sqrt(a.x * a.x + a.y * a.y);
  }
  
  // Dot product of vec2
  float vec2_dot(vec2 a, vec2 b) {
      return a.x * b.x + a.y * b.y;
  }
  
  // Mix between two vec2
  vec2 vec2_mix(vec2 v1, vec2 v2, float a) {
      return (vec2){v1.x * (1.0f - a) + v2.x * a, v1.y * (1.0f - a) + v2.y * a};
  }
  
  // Add two vec3
  vec3 vec3_add(vec3 a, vec3 b) {
      return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
  }
  
  void vec3_add_p(vec3 *a, vec3 b) {
      a->x += b.x;
      a->y += b.y;
      a->z += b.z;
  }
  
  // Add three vec3
  vec3 vec3_add_3(vec3 a, vec3 b, vec3 c) {
      return (vec3){a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z};
  }
  
  // Subtract two vec3
  vec3 vec3_sub(vec3 a, vec3 b) {
      return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
  }
  
  void vec3_sub_p(vec3 *a, vec3 b) {
      a->x -= b.x;
      a->y -= b.y;
      a->z -= b.z;
  }
  
  // Multiply two vec3
  vec3 vec3_mul(vec3 a, vec3 b) {
      return (vec3){a.x * b.x, a.y * b.y, a.z * b.z};
  }
  
  // Scale vec3 by scalar
  vec3 vec3_scale(vec3 a, float s) {
      return (vec3){a.x * s, a.y * s, a.z * s};
  }
  
  void vec3_scale_p(vec3 *a, float s) {
      a->x *= s;
      a->y *= s;
      a->z *= s;
  }
  
  // Calculate length of vec3
  float vec3_length(vec3 a) {
      return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
  }
  
  // Absolute value of vec3 components
  vec3 vec3_abs(vec3 a) {
      return (vec3){fabs(a.x), fabs(a.y), fabs(a.z)};
  }
  
  // Dot product of vec3
  float vec3_dot(vec3 a, vec3 b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
  }
  
  // Cross product of two vec3
  vec3 vec3_cross(vec3 v1, vec3 v2) {
      return (vec3){v1.y * v2.z - v1.z * v2.y,
                    v1.z * v2.x - v1.x * v2.z,
                    v1.x * v2.y - v1.y * v2.x};
  }
  
  // Negate vec3
  vec3 vec3_negate(vec3 v) {
      return (vec3){-v.x, -v.y, -v.z};
  }
  
  // Mix between two vec3
  vec3 vec3_mix(vec3 v1, vec3 v2, float a) {
      return (vec3){v1.x * (1.0f - a) + v2.x * a,
                    v1.y * (1.0f - a) + v2.y * a,
                    v1.z * (1.0f - a) + v2.z * a};
  }
  
  // Mix between two vec4
  vec4 vec4_mix(vec4 v1, vec4 v2, float a) {
      return (vec4){v1.x * (1.0f - a) + v2.x * a,
                    v1.y * (1.0f - a) + v2.y * a,
                    v1.z * (1.0f - a) + v2.z * a,
                    v1.k * (1.0f - a) + v2.k * a};
  }
  
  // Utility Functions
  
  // Clamp value between minValue and maxValue
  float clamp(float value, float minValue, float maxValue) {
      return fmaxf(minValue, fminf(value, maxValue));
  }
  
  // Fractional part of value
  float fract(float x) {
      return x - floor(x);
  }
  
  // Step function
  float step(float edge, float x) {
      return (x >= edge) ? 1.0f : 0.0f;
  }
  
  // Distance between two vec3
  float distance(vec3 v1, vec3 v2) {
      return sqrtf((v2.x - v1.x) * (v2.x - v1.x) +
                   (v2.y - v1.y) * (v2.y - v1.y) +
                   (v2.z - v1.z) * (v2.z - v1.z));
  }
  
  
  vec2 vec2_normalize(vec2* a) {
    float length = vec2_length(*a);
    return (vec2){a->x / length, a->y / length};
  }
  
  void vec2_normalize_p(vec2* a) {
    float length = vec2_length(*a);
    a->x = a->x/length;
    a->y = a->y/length;
  }
  
  
  vec3 vec3_normalize_l(vec3 a) {
    float length = vec3_length(a);
    return (vec3){a.x / length, a.y / length, a.z / length};
  }
  
  vec3 vec3_normalize(vec3 * a) {
    float length = vec3_length(*a);
    return (vec3){a->x / length, a->y / length, a->z / length};
  }
  
  void vec3_normalize_p(vec3 * a) {
    float length = vec3_length(*a); 
    a->x = a->x / length;
    a->y = a->y / length;
    a->z = a->z / length;
  }
  
  
  vec3 vec3_reflect(vec3 * I, vec3 * N) {
    vec3 result;
    float dotProduct = vec3_dot(*I, *N);   // Calculate dot product of I and N
    float scaleFactor = 2.0f * dotProduct; // Multiply dot product by 2
  
    // Reflect vector I based on normal N
    result.x = I->x - scaleFactor * N->x;
    result.y = I->y - scaleFactor * N->y;
    result.z = I->z - scaleFactor * N->z;
  
    return result;
  }
  
  void vec3_reflect_p(vec3 * I, vec3 * N) {
    vec3 result;
    float dotProduct = vec3_dot(*I, *N);   // Calculate dot product of I and N
    float scaleFactor = 2.0f * dotProduct; // Multiply dot product by 2
  
    // Reflect vector I based on normal N
    I->x = I->x - scaleFactor * N->x;
    I->y = I->y - scaleFactor * N->y;
    I->z = I->z - scaleFactor * N->z;
  }
  
  //end math code