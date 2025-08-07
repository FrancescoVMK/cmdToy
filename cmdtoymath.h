#ifndef CMDTOY_MATH_H
#define CMDTOY_MATH_H


// --- Struct Definitions ---

typedef struct {
    float x, y, z, k;
} vec4;

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    float x, y;
} vec2;

typedef struct {
    float x1, y1, x2, y2;
} mat2;

typedef struct Point {
    float dist;
    vec4 color;
    int shapeId;
} Point;

// --- Macro Definitions for Mathematical Calculations ---

extern const float PI;


#define FLOAT_MIX(x, y, a) ((x) * (1.0f - (a)) + (y) * (a))

#define MAT2_TIMES_VEC2(a, b) ((vec2){(a).x1 * (b).x + (a).y1 * (b).y, (a).x2 * (b).x + (a).y2 * (b).y})

#define VEC2_ADD(a, b) ((vec2){(a).x + (b).x, (a).y + (b).y})
#define VEC2_ADD_P(a, b) do { (a).x = (a).x + (b).x; (a).y = (a).y + (b).y; } while(0)

#define VEC2_ADD_3(a, b, c) ((vec2){(a).x + (b).x + (c).x, (a).y + (b).y + (c).y})

#define VEC2_SUB(a, b) ((vec2){(a).x - (b).x, (a).y - (b).y})
#define VEC2_SUB_P(a, b) do { (a).x = (a).x - (b).x; (a).y = (a).y - (b).y; } while(0)

#define VEC2_MUL(a, b) ((vec2){(a).x * (b).x, (a).y * (b).y})
#define VEC2_DIV(a, b) ((vec2){(a).x / (b).x, (a).y / (b).y})

#define VEC2_SCALE(a, s) ((vec2){(a).x * (s), (a).y * (s)})
#define VEC2_SCALE_P(a, s) do { (a).x = (a).x * (s); (a).y = (a).y * (s); } while(0)

#define VEC2_LENGTH(a) (sqrtf((a).x * (a).x + (a).y * (a).y))

#define VEC2_DOT(a, b) ((a).x * (b).x + (a).y * (b).y)
#define VEC2_MIX(v1, v2, a) ((vec2){(v1).x * (1.0f - (a)) + (v2).x * (a), (v1).y * (1.0f - (a)) + (v2).y * (a)})

// vec3 Operations
#define VEC3_ADD(a, b) ((vec3){(a).x + (b).x, (a).y + (b).y, (a).z + (b).z})
#define VEC3_ADD_P(a, b) do { (a).x = (a).x + (b).x; (a).y = (a).y + (b).y; (a).z = (a).z + (b).z; } while(0)

#define VEC3_ADD_3(a, b, c) ((vec3){(a).x + (b).x + (c).x, (a).y + (b).y + (c).y, (a).z + (b).z + (c).z})

#define VEC3_SUB(a, b) ((vec3){(a).x - (b).x, (a).y - (b).y, (a).z - (b).z})
#define VEC3_SUB_P(a, b) do { (a).x = (a).x - (b).x; (a).y = (a).y - (b).y; (a).z = (a).z - (b).z; } while(0)

#define VEC3_MUL(a, b) ((vec3){(a).x * (b).x, (a).y * (b).y, (a).z * (b).z})

#define VEC3_SCALE(a, s) ((vec3){(a).x * (s), (a).y * (s), (a).z * (s)})
#define VEC3_SCALE_P(a, s) do { (a).x = (a).x * (s); (a).y = (a).y * (s); (a).z =  (a).z * (s); } while(0)

#define VEC3_LENGTH(a) (sqrtf((a).x * (a).x + (a).y * (a).y + (a).z * (a).z))

#define VEC3_ABS(a) ((vec3){fabsf((a).x), fabsf((a).y), fabsf((a).z)})

#define VEC3_DOT(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define VEC3_CROSS(v1, v2) ((vec3){(v1).y * (v2).z - (v1).z * (v2).y, \
                     (v1).z * (v2).x - (v1).x * (v2).z, \
                     (v1).x * (v2).y - (v1).y * (v2).x})
#define VEC3_NEGATE(v) ((vec3){-(v).x, -(v).y, -(v).z})
#define VEC3_MIX(v1, v2, a) ((vec3){(v1).x * (1.0f - (a)) + (v2).x * (a), \
                                      (v1).y * (1.0f - (a)) + (v2).y * (a), \
                                      (v1).z * (1.0f - (a)) + (v2).z * (a)})

// vec4 Operations
#define VEC4_MIX(v1, v2, a) ((vec4){(v1).x * (1.0f - (a)) + (v2).x * (a), \
                                      (v1).y * (1.0f - (a)) + (v2).y * (a), \
                                      (v1).z * (1.0f - (a)) + (v2).z * (a), \
                                      (v1).k * (1.0f - (a)) + (v2).k * (a)})

// Utility Functions
#define CLAMP(value, minValue, maxValue) (fmaxf(minValue, fminf(value, maxValue)))
#define FRACT(x) ((x) - floorf(x)) // Use floorf for float
#define STEP(edge, x) ((x) >= (edge) ? 1.0f : 0.0f)

#define DISTANCE(v1, v2) (sqrtf(((v2).x - (v1).x) * ((v2).x - (v1).x) + \
                  ((v2).y - (v1).y) * ((v2).y - (v1).y) + \
                  ((v2).z - (v1).z) * ((v2).z - (v1).z)))


// --- Function Prototypes ---

float float_mix(float x, float y, float a);

// Multiply mat2 with vec2
vec2 mat2_times_vec2(mat2 a, vec2 b);

// Add two vec2
vec2 vec2_add(vec2 a, vec2 b);
// Add two vec2 in place
void vec2_add_p(vec2 *a, vec2 b);

// Add three vec2
vec2 vec2_add_3(vec2 a, vec2 b, vec2 c);

// Subtract two vec2
vec2 vec2_sub(vec2 a, vec2 b);
// Subtract two vec2 in place
void vec2_sub_p(vec2 *a, vec2 b);

// Multiply two vec2
vec2 vec2_mul(vec2 a, vec2 b);

// Divide two vec2
vec2 vec2_div(vec2 a, vec2 b);

// Scale vec2 by scalar
vec2 vec2_scale(vec2 a, float s);
// Scale vec2 by scalar in place
void vec2_scale_p(vec2 *a, float s);

// Calculate length of vec2
float vec2_length(vec2 a);

// Dot product of vec2
float vec2_dot(vec2 a, vec2 b);

// Mix between two vec2
vec2 vec2_mix(vec2 v1, vec2 v2, float a);

// Normalize vec2 (returns new vec2)
vec2 vec2_normalize(vec2* a);
// Normalize vec2 in place
void vec2_normalize_p(vec2* a);


// Add two vec3
vec3 vec3_add(vec3 a, vec3 b);
// Add two vec3 in place
void vec3_add_p(vec3 *a, vec3 b);

// Add three vec3
vec3 vec3_add_3(vec3 a, vec3 b, vec3 c);

// Subtract two vec3
vec3 vec3_sub(vec3 a, vec3 b);
// Subtract two vec3 in place
void vec3_sub_p(vec3 *a, vec3 b);

// Multiply two vec3
vec3 vec3_mul(vec3 a, vec3 b);

// Scale vec3 by scalar
vec3 vec3_scale(vec3 a, float s);
// Scale vec3 by scalar in place
void vec3_scale_p(vec3 *a, float s);

// Calculate length of vec3
float vec3_length(vec3 a);

// Absolute value of vec3 components
vec3 vec3_abs(vec3 a);

// Dot product of vec3
float vec3_dot(vec3 a, vec3 b);

// Cross product of two vec3
vec3 vec3_cross(vec3 v1, vec3 v2);

// Negate vec3
vec3 vec3_negate(vec3 v);

// Mix between two vec3
vec3 vec3_mix(vec3 v1, vec3 v2, float a);

// Normalize vec3 (returns new vec3)
vec3 vec3_normalize_l(vec3 a); // Original function name
vec3 vec3_normalize(vec3 * a); // Function taking pointer
// Normalize vec3 in place
void vec3_normalize_p(vec3 * a);

// Reflect vector I based on normal N (returns new vec3)
vec3 vec3_reflect(vec3 * I, vec3 * N);
// Reflect vector I based on normal N in place
void vec3_reflect_p(vec3 * I, vec3 * N);


// Mix between two vec4
vec4 vec4_mix(vec4 v1, vec4 v2, float a);

// Utility Functions

// Clamp value between minValue and maxValue
float clamp(float value, float minValue, float maxValue);

// Fractional part of value
float fract(float x);

// Step function
float step(float edge, float x);

// Distance between two vec3
float distance(vec3 v1, vec3 v2);

#endif // MATH_UTILS_H