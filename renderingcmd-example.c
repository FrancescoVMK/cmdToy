#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <uchar.h>
#include <pthread.h>

//structs

typedef struct {
  float x, y, z, k;
} vec4; 

typedef struct {
  float x, y, z;
} vec3;

typedef struct {
  float x, y;
} vec2;

typedef struct{
  float x1, y1, x2, y2;
} mat2;

typedef struct Point{
  float dist;
  vec4 color;
  int shapeId;
} Point;


//render

//CONSTANTS
const float PI = 3.141592653589793;
//print
const char* g_ansi_color_reset = "\x1b[0m"; //len 4
const char *ansi_colors[16] = { //5 char lenght
  "\033[30m",  //Black
  "\033[34m",  //Blue
  "\033[32m",  //Green
  "\033[36m",  //Cyan
  "\033[31m",  //Red
  "\033[35m",  //Magenta
  "\033[33m",  //Yellow
  "\033[37m",  //White

  "\033[90m",  //Bright Black (Gray)
  "\033[94m",  //Bright Blue
  "\033[92m",  //Bright Green
  "\033[96m",  //Bright Cyan
  "\033[91m",  //Bright Red
  "\033[95m",  //Bright Magenta   
  "\033[93m",  //Bright Yellow
  "\033[97m",  //Bright White


};
const int g_pixeLenght = 11; //lenght of the string that rapresents a pixel, in this case: color ansi code 5 chars + 2 chars + end ansi code 4 chars 

//const char gscale[183] = " .`:,;'_^></-!~=*)(|j?}{][ti+l7v1ryfcJ32uIC$TYzoasnVw96g5qpkOL40&mG8xhedbZUSAQPFDXWK#RNEHBM@.`:,;'_^></-!~=*)(|j?}{][ti+l7v1ryfcJ32uIC$zwoTYasnV96g5qpkOL40&mG8xhedbZUSAQPFDXWK#RNEHBM@";
const char g_gscale[41] = " ,.~!+:*Ivcow0XP$#RB@,.~!+:*vcowI0XP$#RB@";
//const char gscale[70] = " .`:,;'_^\"\\></-!~=)(|j?}{][ti+l7v1%yrfcJ32uIC$zwo96sngaT5qpkYVOL40&mG8*xhedbZUSAQPFDXWK#RNEHBM@";


//globals
static struct winsize w;
static clock_t beginTime ;
static float iTime;
static vec2 iMouse = (vec2){0., 0.};
static vec2 iResolution = (vec2){10., 10.};

static int g_maxThreadNumber = 10;
static int g_thread_rows = 0;
static int g_thread_columns = 0;

static int g_bufferLenght = 0;
static char* g_printBuffer;
static char* g_printBufferToPrint;
static pthread_t g_printThread;

//math functions baldy implemented, should also be saparated in a library or something


//define macros for matemtical calulations
#define FLOAT_MIX(x, y, a) x * (1.0f - a) + y * a

#define MAT2_TIMES_VEC2(a, b) ((vec2){a.x1 * b.x + a.y1 * b.y, a.x2 * b.x + a.y2 * b.y})

#define VEC2_ADD(a, b) ((vec2){(a).x + (b).x, (a).y + (b).y})
#define VEC2_ADD_P(a, b) (a).x = (a).x + (b).x; (a).y = (a).y + (b).y;

#define VEC2_ADD_3(a, b, c) ((vec2){(a).x + (b).x + (c).x, (a).y + (b).y + (c).y})

#define VEC2_SUB(a, b) ((vec2){(a).x - (b).x, (a).y - (b).y})
#define VEC2_SUB_P(a, b) (a).x  = (a).x - (b).x; (a).y = (a).y - (b).y

#define VEC2_MUL(a, b) ((vec2){(a).x * (b).x, (a).y * (b).y})
#define VEC2_DIV(a, b) ((vec2){(a).x / (b).x, (a).y / (b).y})

#define VEC2_SCALE(a, s) ((vec2){(a).x * (s), (a).y * (s)})
#define VEC2_SCALE_P(a, s) (a).x = (a).x * (s); (a).y = (a).y * (s)

#define VEC2_LENGTH(a) (sqrt((a).x * (a).x + (a).y * (a).y))

//#define VEC2_NORMALIZE(a) ((vec2){(a).x / VEC2_LENGTH(a), (a).y / VEC2_LENGTH(a)}) use function


#define VEC2_DOT(a, b) ((a).x * (b).x + (a).y * (b).y)
#define VEC2_MIX(v1, v2, a) ((vec2){(v1).x * (1.0f - (a)) + (v2).x * (a), (v1).y * (1.0f - (a)) + (v2).y * (a)})

// vec3 Operations
#define VEC3_ADD(a, b) ((vec3){(a).x + (b).x, (a).y + (b).y, (a).z + (b).z})
#define VEC3_ADD_P(a, b) (a).x = (a).x + (b).x; (a).y = (a).y + (b).y; (a).z = (a).z + (b).z

#define VEC3_ADD_3(a, b, c) ((vec3){(a).x + (b).x + (c).x, (a).y + (b).y + (c).y, (a).z + (b).z + (c).z})

#define VEC3_SUB(a, b) ((vec3){(a).x - (b).x, (a).y - (b).y, (a).z - (b).z})
#define VEC3_SUB_P(a, b) (a).x = (a).x - (b).x; (a).y = (a).y - (b).y; (a).z = (a).z - (b).z

#define VEC3_MUL(a, b) ((vec3){(a).x * (b).x, (a).y * (b).y, (a).z * (b).z})

#define VEC3_SCALE(a, s) ((vec3){(a).x * (s), (a).y * (s), (a).z * (s)})
#define VEC3_SCALE_P(a, s) (a).x = (a).x * (s); (a).y = (a).y * (s); (a).z =  (a).z * (s)

#define VEC3_LENGTH(a) (sqrt((a).x * (a).x + (a).y * (a).y + (a).z * (a).z))
//#define VEC3_NORMALIZE(a) ((vec3){(a).x / VEC3_LENGTH(a), (a).y / VEC3_LENGTH(a), (a).z / VEC3_LENGTH(a)}) use function

#define VEC3_ABS(a) ((vec3){fabs((a).x), fabs((a).y), fabs((a).z)})

#define VEC3_DOT(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define VEC3_CROSS(v1, v2) ((vec3){(v1).y * (v2).z - (v1).z * (v2).y,	\
				   (v1).z * (v2).x - (v1).x * (v2).z,	\
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
#define FRACT(x) ((x) - floor(x))

#define STEP(edge, x) ((x) >= (edge) ? 1.0f : 0.0f)

//use function
/* #define REFLECT(I, N) ((vec3){(I).x - 2.0f * VEC3_DOT(I, N) * (N).x, \
   (I).y - 2.0f * VEC3_DOT(I, N) * (N).y, \
   (I).z - 2.0f * VEC3_DOT(I, N) * (N).z})
*/


#define DISTANCE(v1, v2) (sqrtf(((v2).x - (v1).x) * ((v2).x - (v1).x) + \
				((v2).y - (v1).y) * ((v2).y - (v1).y) + \
				((v2).z - (v1).z) * ((v2).z - (v1).z)))




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

vec3 vec3_reflect_p(vec3 * I, vec3 * N) {
  vec3 result;
  float dotProduct = vec3_dot(*I, *N);   // Calculate dot product of I and N
  float scaleFactor = 2.0f * dotProduct; // Multiply dot product by 2

  // Reflect vector I based on normal N
  I->x = I->x - scaleFactor * N->x;
  I->y = I->y - scaleFactor * N->y;
  I->z = I->z - scaleFactor * N->z;
}

//end math code

//shader code

static float g_angle = 0;
static vec3 g_lightSource = (vec3){10., 9., 10.};

const vec4 g_backGroundColor = (vec4){0.4,0.4,1.0,1.0};

const vec3 g_lightColorSpecular = (vec3){1.000,1.000,1.000};
const vec3 g_lightColorDiffuse = (vec3){1.000,1.000,1.000};
const vec3 g_ambient = (vec3){1.000,1.000,1.000};

const float g_max_dist = 20.0;
const float g_min_dist = 0.01;

const float g_speculareScale = 1.0;
const float g_diffiuseScale = 0.7;
const float g_ambientscale = 0.5;

//shapes 
Point sdfSphere(vec3 p, vec3 center, float radius, vec4 color, vec3 rotation, vec3 pivot, int currentId){


  // n must be normalized
  
  p = vec3_sub(p, center);//center


  //rotation matrix
  mat2 rotX = (mat2){cos(rotation.x), -sin(rotation.x),
		     sin(rotation.x),  cos(rotation.x)};
    
  vec2 yz = (vec2){p.y, p.z};
  yz = mat2_times_vec2(rotX, yz);//rotX * p.yz;
  p.y = yz.x;
  p.z = yz.y;

  mat2 rotY = (mat2){cos(rotation.y), sin(rotation.y),
		     -sin(rotation.y), cos(rotation.y)};

  vec2 xz = (vec2){p.x, p.z};
  xz = mat2_times_vec2(rotY, xz);
  p.x = xz.x;
  p.z = xz.y;

  mat2 rotZ = (mat2){cos(rotation.z), -sin(rotation.z),
		     sin(rotation.z),  cos(rotation.z)};

  vec2 xy = (vec2){p.x, p.y}; 
  xy = mat2_times_vec2(rotZ, xy);//rotZ * p.xy;
  p.x = xy.x;
  p.y = xy.y;
  

  p = vec3_add(p, pivot);//p += pivot;//apply pivot
    
  Point res = (Point){vec3_length(vec3_sub(p, center)) - radius, color, currentId};

  return res;
}

Point sdTorus(vec3 p, vec3 center, vec2 t, vec4 color, vec3 rotation, vec3 pivot, int currentId){

  // n must be normalized
  p = vec3_sub(p, center);//center


    //rotation matrix
  mat2 rotX = (mat2){cos(rotation.x), -sin(rotation.x),
                        sin(rotation.x),  cos(rotation.x)};
  vec2 yz = mat2_times_vec2(rotX, (vec2){p.y, p.z});//rotX * p.yz;
  p.y = yz.x;
  p.z = yz.y;

  mat2 rotY = (mat2){cos(rotation.y), sin(rotation.y),
                      -sin(rotation.y), cos(rotation.y)};
  vec2 xz = mat2_times_vec2(rotY, (vec2){p.x, p.z});
  p.x = xz.x;
  p.z = xz.y;

  mat2 rotZ = (mat2){cos(rotation.z), -sin(rotation.z),
                        sin(rotation.z),  cos(rotation.z)};
  vec2 xy = mat2_times_vec2(rotZ, (vec2){p.x, p.y});//rotZ * p.xy;
  p.x = xy.x;
  p.y = xy.y;



  p = vec3_add(p, pivot);//p += pivot;//apply pivot
  
  Point res = (Point){vec2_length( (vec2){vec2_length((vec2){p.x, p.z}) - t.x, p.y} )  - t.y, color, currentId};

  return res;
}


Point sdfPlane( vec3 p, vec3 n, float h, vec4 color, int currentId){
  // n must be normalized
  
  
  return (Point){n.x * p.x + n.y * p.y + n.z * p.z - h, color, currentId};
}


Point sdfBox( vec3 p, vec3 b, vec3 center, vec4 color, vec3 base1, vec3 base2, vec3 base3, vec3 rotation, vec3 pivot, vec3 deform1, vec3 deform2, vec3 deform3, vec3 deformPivot, int currentId){
  
  
  p = vec3_sub(p, center);//center

  //change base

  vec3_scale_p(&base1, p.x);
  vec3_scale_p(&base2, p.y);
  vec3_scale_p(&base3, p.z);

  p = vec3_add_3(base1, base2, base3);

  //rotation matrix
  mat2 rotX = (mat2){cos(rotation.x), -sin(rotation.x),
		     sin(rotation.x),  cos(rotation.x)};
    
  vec2 yz = (vec2){p.y, p.z};
  yz = mat2_times_vec2(rotX, yz);//rotX * p.yz;
  p.y = yz.x;
  p.z = yz.y;

  mat2 rotY = (mat2){cos(rotation.y), sin(rotation.y),
		     -sin(rotation.y), cos(rotation.y)};

  vec2 xz = (vec2){p.x, p.z};
  xz = mat2_times_vec2(rotY, xz);
  p.x = xz.x;
  p.z = xz.y;

  mat2 rotZ = (mat2){cos(rotation.z), -sin(rotation.z),
		     sin(rotation.z),  cos(rotation.z)};

  vec2 xy = (vec2){p.x, p.y}; 
  xy = mat2_times_vec2(rotZ, xy);//rotZ * p.xy;
  p.x = xy.x;
  p.y = xy.y;
  

    
  p = vec3_add(p, pivot);//apply pivot
    
  vec3 q = vec3_sub(vec3_abs(p), b);

  vec3_scale_p(&deform1, q.x); 
  vec3_scale_p(&deform2, q.y);
  vec3_scale_p(&deform3, q.z);

  q = vec3_add_3(deform1, deform2, deform3);//q.x * deform1 + q.y * deform2 + q.z * deform3;
  vec3_add_p(&q, deformPivot); 


  return (Point){vec3_length((vec3){fmaxf(q.x, 0.0), fmaxf(q.y, 0.0), fmaxf(q.z, 0.0)}) + fminf(fmaxf(q.x,fmaxf(q.y,q.z)),0.0), color, currentId};
}


//functions

Point opSmoothUnion(Point d1, Point d2, float k){

  float h = clamp(0.5 + 0.5 * (d2.dist - d1.dist) / k, 0.0, 1.0);
    
  vec4 color = vec4_mix(d2.color, d1.color, h);
    
  int shapeId = h > 0.5 ? d1.shapeId : d2.shapeId;
    
  return (Point){float_mix(d2.dist, d1.dist, h) - k * h * (1.0 - h), color, shapeId};
}
 

Point map(vec3 p, int jumpShape){

  int shapeIndex = 1;
    
  Point m = sdfPlane(p, 
		     (vec3){0.0, 1.0, 0.0}, 
		     -1.0, 
		     (vec4){0.0, 1.0, 0.0, 1.0},
		     shapeIndex
		     );
    
  shapeIndex++;
    
  Point obj;
    

  /*     if(shapeIndex != jumpShape){
         obj = sdfSphere(p,
	 (vec3){0.0, 0.0, 0.0},
	 1.5,
	 (vec4){1.000, 0.000, 0.000,1.0},
	 //rotate
	 (vec3){0., 0., 0.}, //rotation
	 (vec3){0., 0., 0.}, //pivot
	 shapeIndex
	 );
         m = opSmoothUnion(obj, m, 0.1);
	 }

	 shapeIndex++; */
    
  /*   if(shapeIndex != jumpShape){
    

       obj = sdfBox(p, 
       (vec3){0.3, 0.3, 0.3}, 

       (vec3){0.0, 0.0, 0.0},

       (vec4){0.0, 0.0, 1.0, 1.0},

       //base
       (vec3){1.0, 0.0, 0.0},
       (vec3){0.0, 1.0, 0.0},
       (vec3){0.0, 0.0, 1.0},

       //rotate
       (vec3){0., 0.,  0.}, //rotation
       (vec3){0., 0., 0.}, //pivot

       //deform
       (vec3){1.0, 0.0, 0.0},
       (vec3){0.0, 1.0, 0.0},
       (vec3){0.0, 0.0, 1.0},

       (vec3){0.0, 0.0, 0.0}, //deformPivot

       shapeIndex
       );

       m = opSmoothUnion(obj, m, 0.1);
       }
       shapeIndex++;  */

  
  if(shapeIndex != jumpShape){
    obj = sdTorus(p,
		  (vec3){0.0, 0.5, 0.0},
		  (vec2){1., 0.4},
		  (vec4){1.000, 0.000, 0.000,1.0},
		  //rotate
		  (vec3){0., 0., 0.4 * PI}, //rotation
		  (vec3){0., 0., 0.}, //pivot
		  shapeIndex
		  );
    m = opSmoothUnion(obj, m, 0.2);
  }

  shapeIndex++;



  if(shapeIndex != jumpShape){
    

    obj = sdfBox(p, 
		 (vec3){0.3, 0.3, 0.3}, 

		 (vec3){sin(iTime)*1.5, sin(iTime)*1.5 + 0.5, cos(iTime)*1. + 1.0},

		 (vec4){0.0, 0.0, 1.0, 0.5},

		 //base
		 (vec3){1.0, 0.0, 0.0},
		 (vec3){0.0, 1.0, 0.0},
		 (vec3){0.0, 0.0, 1.0},

		 //rotate
		 (vec3){-g_angle, 0.,  g_angle}, //rotation
		 (vec3){0., 0., 0.}, //pivot

		 //deform
		 (vec3){1.0, 0.0, 0.0},
		 (vec3){0.0, 1.0, 0.0},
		 (vec3){0.0, 0.0, 1.0},

		 (vec3){0.0, 0.0, 0.0}, //deformPivot

		 shapeIndex
		 );

    m = opSmoothUnion(obj, m, 0.2);
  }
  shapeIndex++;

  
    
  return m;
}

vec3 getNormal(vec3 p){
  vec2 e = (vec2){0.0001, -0.0001};
  int j = -1;

  vec3 xxx = (vec3){e.x, e.x, e.x};
  vec3 xyy = (vec3){e.x, e.y, e.y};
  vec3 yxy = (vec3){e.y, e.x, e.y};
  vec3 yyx = (vec3){e.y, e.y, e.x};

  vec3_scale_p(&xxx, map(vec3_add(p, xxx),j).dist);
  vec3_scale_p(&xyy, map(vec3_add(p, xyy),j).dist);
  vec3_scale_p(&yxy, map(vec3_add(p, yxy),j).dist);
  vec3_scale_p(&yyx, map(vec3_add(p, yyx),j).dist);

  vec3 sumall = vec3_normalize(&(vec3){
    xxx.x + xyy.x + yxy.x + yyx.x,
    xxx.y + xyy.y + yxy.y + yyx.y,
    xxx.z + xyy.z + yxy.z + yyx.z,
  });

  return sumall;
}

Point rayMarch(vec3 ro, vec3 rd, float maxDist){

    
  int jShapes = -1;
    
  Point p = map(ro, jShapes);
  float dist = p.dist;
  Point oldP = (Point){g_max_dist, g_backGroundColor, -1};
    
  int i = 0;
    
    
  while(dist < maxDist && i < 200){
    i++;
    p = map(vec3_add(ro, vec3_scale(rd, dist)), jShapes);
    if(p.dist <= g_min_dist){
      if(p.color.k >= 1.0){
        if(oldP.shapeId < 0){
          oldP.dist = dist;
          oldP.color = p.color;
          oldP.shapeId = p.shapeId;
        }else{
          p.color = vec4_mix(p.color, oldP.color, oldP.color.k);
          oldP.color = p.color;
        }
                
        break;

      }else{
        p.color = vec4_mix(oldP.color, p.color, p.color.k);
        oldP.color = p.color;
        if(oldP.dist >= g_max_dist){
          oldP.dist = dist;
                        
          oldP.shapeId = p.shapeId;
        }
        jShapes = p.shapeId;
      }
    }

    dist += p.dist;
  }
  
  return oldP;

}



Point rayMarchShadow(vec3 ro, vec3 rd, float maxDist, int currentShapeId){

  float t = 0.1;
  Point minDistP = (Point){maxDist, g_backGroundColor, -1};
  for( int i=0; i<256 && t<maxDist; i++ ){
    Point h = map(vec3_add(ro, vec3_scale(rd, t)), currentShapeId);

    if(h.dist < g_min_dist)
      return h;
             
        
    t += h.dist;
  }
    
  return minDistP;


}


void camera(vec2* uv, vec3* ro, vec3* rd){  //out vec3 ro, out vec3 rd
  //camera from https://www.shadertoy.com/view/XsXXDB
  static const vec3 zoom = (vec3){0.0, 6.0, 0.0};
  static const vec3 ta = (vec3){0.0, 0.0, 0.0};
  static const vec3 cp = (vec3){0.0, 1.0, 0.0 };
    
  vec2 m = vec2_div(iMouse , iResolution);

  float hd = -m.x * 14.0 + 3.14159;
  float elv = m.y * 3.14159 * 0.4 - 3.14159 * 0.25;
    
  *ro = (vec3){sin(hd) * cos(elv), sin(elv), cos(hd) * cos(elv)};

  vec3_scale_p(ro, 8.0);
    
  vec3_add_p(ro, zoom); 
    

  // camera tx
  vec3 cw = vec3_normalize_l(vec3_sub(ta, *ro));
  vec3 cu = vec3_normalize_l(vec3_cross(cw, cp));
  vec3 cv = vec3_normalize_l(vec3_cross(cu, cw));
    
  vec3_scale_p(&cw, 2.5);
  vec3_scale_p(&cu,uv->x);
  vec3_scale_p(&cv, uv->y);
    
  *rd = vec3_normalize_l(vec3_add_3(cu, cv, cw)); 
}

vec3 render(vec2* uv){
    

  vec3 ro;
  vec3 rd;
  vec3 color;
    
  camera(uv, &ro, &rd);
    
  Point p = rayMarch(ro, rd, g_max_dist);
    
    
  if (p.dist >= g_max_dist){
      return (vec3){p.color.x, p.color.y, p.color.z};
  }
    
  //color = vec3(1.0);
    
  //normals
  vec3_scale_p(&rd, p.dist);
  vec3 pos = vec3_add(ro, rd); //get point wher you need to calculate the normals
  vec4 baseColor = p.color;
  vec3 normal = getNormal(pos);
    
    


  //lighting

    
  //ambient

    
  //diffuse

  //float angle = (iTime * 0.05) * PI * 2.;
  //vec3 lightSource = (vec3){sin(angle) * 10., 9., cos(angle) * 10.};
    
    
  float diffuseStrength = fmaxf(0.0, vec3_dot(vec3_normalize(&g_lightSource),normal));
    
  vec3 diffuse = vec3_scale(g_lightColorDiffuse, diffuseStrength);
    
  //specular
    
    
  vec3 viewSource = vec3_normalize(&ro);
  vec3 reflectSource = vec3_negate(g_lightSource);
  vec3_reflect_p(&reflectSource ,&normal);
  vec3_normalize_p(&reflectSource);

  float specularStrength = fmaxf(0.0, vec3_dot(viewSource, reflectSource));
  specularStrength = pow(specularStrength, 64.0);
  vec3 specular = vec3_scale(g_lightColorSpecular, specularStrength);

  vec3_scale_p(&diffuse, g_diffiuseScale);
  vec3_scale_p(&specular, g_speculareScale);
  vec3 lighting = vec3_add_3( diffuse, specular, vec3_scale(g_ambient, g_ambientscale)); 
        
  color = vec3_mul((vec3){baseColor.x, baseColor.y, baseColor.z}, lighting);

  //shadows
  if(diffuseStrength > 0.0){
    vec3 lightDirection = vec3_normalize(&g_lightSource);
    float distToLightSource = distance(g_lightSource, pos);
    ro = pos;
    rd = lightDirection;
    Point pointShadow = rayMarchShadow(ro, rd, distToLightSource, p.shapeId);


    if(pointShadow.dist < 1.){

      color = vec3_mix(vec3_scale((vec3){pointShadow.color.x, pointShadow.color.y, pointShadow.color.z}, pointShadow.color.k), vec3_scale(color, pointShadow.dist),  pointShadow.color.k);

    }   
  }

  //gamma correction
  float gam = 1.0 / 2.2;
  color.x = powf(color.x, gam);
  color.y = powf(color.y, gam);
  color.z = powf(color.z, gam);


  // Tone mapping
  //color = 1.-exp(-2.*color);
        
  
  return color;
}


vec3 mainImage(vec2 * fragCoord){
  vec3 fragColor; //out, its vec3 and not vec4 like in shader toy
  // Calculate aspect ratio
  float aspectRatio = iResolution.x / iResolution.y;
  //printf("%f", iResolution.x);
  // Normalized pixel coordinates (from 0 to 1)
  vec2 fragCordScaled = vec2_scale(*fragCoord, 2.0);
  fragCordScaled.x = fragCordScaled.x / iResolution.x;
  fragCordScaled.y = fragCordScaled.y / iResolution.y;

  vec2 uv = (vec2){fragCordScaled.x - 1., fragCordScaled.y - 1.};

  // Correct for aspect ratio
  uv.x = uv.x * aspectRatio;

  fragColor = render(&uv);

  fragColor.x = fminf(fragColor.x, 1.);
  fragColor.y = fminf(fragColor.y, 1.);
  fragColor.z = fminf(fragColor.z, 1.);

  return fragColor;
}

//render code
char *colorToColoredChar(vec3* color){
  //static char buf[12]; //store buffer fore result g_pixeLenght + 1 end of string char = 12 //end of string ingored in newer verison so 11
  //buf[0] = '\0'; 
  static char buf[11];
  
  float average = (color->x + color->y + color->z)/3;

  //float averagec = fminf(fmaxf(fmaxf(color.x, color.y), color.z), 1.);
  int indexScale = (int)(average*40); 

  int bright = fminf(round(average * 2.), 1.) * 8;


  int coiorindex = bright + (int)(((int)round(color->x) << 2) | ((int)round(color->y) << 1) | (int)round(color->z));

    
  //printf("%f %f %f \n", color.x,  color.y, color.z);  
  //printf("%d\n", indexScale);  
  //snprintf(buf, sizeof(buf), "%s%c%c%s", ansi_colors[coiorindex], g_gscale[indexScale], g_gscale[indexScale], g_ansi_color_reset);
  memcpy(buf, ansi_colors[coiorindex], 5);
  buf[5] = g_gscale[indexScale];
  buf[6] = g_gscale[indexScale];
  memcpy(buf + 7, g_ansi_color_reset, 4);
  return buf;

}

char *setColorToColoredChar(vec3* color, char* buf){
  float average = (color->x + color->y + color->z)/3;

  //float averagec = fminf(fmaxf(fmaxf(color.x, color.y), color.z), 1.);
  int indexScale = (int)(average*40); 

  int bright = fminf(round(average * 2.), 1.) * 8;


  int coiorindex = bright + (int)(((int)round(color->x) << 2) | ((int)round(color->y) << 1) | (int)round(color->z));

    
  //printf("%f %f %f \n", color.x,  color.y, color.z);  
  //printf("%d\n", indexScale);  
  //snprintf(buf, sizeof(buf), "%s%c%c%s", ansi_colors[coiorindex], g_gscale[indexScale], g_gscale[indexScale], g_ansi_color_reset);
  memcpy(buf, ansi_colors[coiorindex], 5);
  buf[5] = g_gscale[indexScale];
  buf[6] = g_gscale[indexScale];
  memcpy(buf + 7, g_ansi_color_reset, 4);
  return buf;

}

char *getPixel(int x,int y){

  vec2 uv = (vec2){(float)x, (float)y};
  vec3 color = mainImage(&uv);
  return colorToColoredChar(&color);
}

//same as get but you pass the buffer (already offsetted of the position u need)
char *setPixel(int x,int y, char* buf){

  vec2 uv = (vec2){(float)x, (float)y};
  vec3 color = mainImage(&uv);

  return setColorToColoredChar(&color, buf);
}




//preplace the \n in the buffer at the right position inizialising the buffer useful for debugging
void inzialazePrintBuffer(int rows, int columns){
  int index = 0;

  for (int x = 0; x < rows; x++ ) {
    for (int y = 0; y < columns ; y ++) { 
      //printf ("%d\n", (y * x * g_pixeLenght));
      index = x * (columns * g_pixeLenght + 1) + y * g_pixeLenght;
      memcpy(g_printBuffer + index , "\033[91m##\x1b[0m", g_pixeLenght); 
    }
    
    index = (x + 1) * columns  * g_pixeLenght + x;
    g_printBuffer[index] = '\n';
    
  }  

  g_printBuffer[columns* rows * g_pixeLenght + columns] = '\0';
}




pthread_mutex_t lock;

void* startThread(void* arg) {
  int thread_id = (int)(intptr_t)arg;
  int piceSize = (g_thread_rows * g_thread_columns) / g_maxThreadNumber;
  int startCell = thread_id * piceSize;
  int endCell = 0;
  if(thread_id == g_maxThreadNumber - 1) {
    endCell = g_thread_rows * g_thread_columns;
  }else{
    endCell = (thread_id + 1) * piceSize;
  }

  //printf("THREAD: %d\n", thread_id);
  //printf("startCell: %d\n", startCell);
  //printf("endCell: %d\n", endCell);

  for (int cell = startCell; cell < endCell; cell++ ) {
      int x = cell % g_thread_rows;
      int y = cell / g_thread_rows;
      //printf("cell: %d\n", cell);
      //printf("x: %d\n", x);
      //printf("y: %d\n", y);
      //char* res = getPixel(y, g_thread_rows - x);
      //pthread_mutex_lock(&lock);
      //memcpy(g_printBuffer + x * (g_thread_columns * g_pixeLenght + 1) + y * g_pixeLenght , getPixel(y, g_thread_rows - x), g_pixeLenght);
      setPixel(y, g_thread_rows - x, g_printBuffer + x * (g_thread_columns * g_pixeLenght + 1) + y * g_pixeLenght);
      //pthread_mutex_unlock(&lock);
      //printf(g_printBuffer); 
  }
  //printf("THREAD END: %d\n", thread_id);
  return NULL;
}



void* printThreaded() {
  printf(g_printBufferToPrint);
  printf("\033[H"); 
}

void displayGridFrameThread(int rows, int columns) {
  pthread_t threads[g_maxThreadNumber];
  //pthread_mutex_init(&lock, NULL);

  g_thread_rows = rows;
  g_thread_columns = columns;
  for (int i = 0; i < g_maxThreadNumber; i++) {
    if (pthread_create(&threads[i], NULL, startThread, (void*)(intptr_t)i) != 0) {
        perror("Failed to create thread");
        exit(EXIT_FAILURE);
    }
    //getchar();
  }

  for (int i = 0; i < g_maxThreadNumber; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
        perror("Failed to join thread");
        exit(EXIT_FAILURE);
    }
  }

  
  pthread_join(g_printThread, NULL);
  memcpy(g_printBufferToPrint, g_printBuffer, g_bufferLenght);
  pthread_create(&g_printThread, NULL, printThreaded, NULL);

  //printf(g_printBuffer);
  //printf("\033[H"); 

}


void displayGridFrameMemCopy(int rows, int columns) {
  //printf("%s", g_printBuffer);
  //return;
  for (int x = 0; x < rows; x++ ) {
    for (int y = 0; y < columns; y++) { 
      memcpy(g_printBuffer +  x * (columns * g_pixeLenght + 1) + y * g_pixeLenght , getPixel(y, rows - x), g_pixeLenght);
    }
  }

  printf(g_printBuffer);
  printf("\033[H");
}

void displayGridFrame(int rows, int columns) {
    
  for (int x = 0; x < rows; x++ ) {
    for (int y = 0; y < columns; y++) { 
      strcat(g_printBuffer, getPixel(y, rows - x));
    }
    strcat(g_printBuffer, "\n");
  }
  printf(g_printBuffer);
  g_printBuffer[0] = '\0';
  printf("\033[H"); 
}

void displayGridLine(int rows, int columns) {
  
  for (int x = 0; x < rows; x++ ) {
    for (int y = 0; y < columns; y++) { 
      strcat(g_printBuffer, getPixel(y, rows - x));
    }
    
    printf("%s\n", g_printBuffer);
    g_printBuffer[0] = '\0';
  }
  printf("\033[H"); 

}

void displayGridChar(int rows, int columns) {
  for (int x = 0; x < rows; x++) { 
    for (int y = 0; y < columns; y++) { 
      printf("%s",getPixel(y, rows - x));
    }
    //fflush(stdout);
    printf("\n");
  }
  printf("\033[H"); 
}


int main(int argc, char *argv[]) {
    
  //get resolution
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  system("@cls||clear");//clear screen
  int rows = w.ws_row -2;
  int columns = (int)w.ws_col/2;
  iResolution = (vec2){columns, rows + 2}; 

  //show detected size
  printf("lines %d\n", rows);
  printf("columns %d\n", columns);


  struct timespec beginTime, frameDebugTime;
    
  //chose print mode
  void (*display)();
  int mode = 4;
  if(argc >= 2){
    mode = atoi(argv[1]);
  }

  

  switch(mode){
    //allocate memory for print buffer if used (+1 column for the \n) + 1 for end char + 1 fpr end \n
    case 0: //char mode
      printf("0 display char mode\n");
      display = displayGridChar;
      break;
    case 1: //line mode
      printf("1 display line mode\n");
      g_bufferLenght = (columns + 1) * g_pixeLenght;
      display = displayGridLine;
      break;
    case 2: //frame mode
      printf("2 display frame mode str concat\n");

      g_bufferLenght = (columns + 1) * rows * g_pixeLenght;
      
      display = displayGridFrame;
      break;

    case 3: //frame mode
      printf("3 display frame mode mem copy\n");

      g_bufferLenght = (columns + 1) * rows * g_pixeLenght;
      display = displayGridFrameMemCopy;
      break;
      
    case 4://theread mode
      printf("4 display frame thread mode\n");

      g_bufferLenght = (columns + 1) * rows * g_pixeLenght;

    
      display = displayGridFrameThread;

      if(argc >= 3){
	    g_maxThreadNumber = atoi(argv[2]);
      }
      printf("Max threads number: %d \n",  g_maxThreadNumber);
      //pre print
      g_printBufferToPrint = (char *)malloc((columns + 1) * rows * g_pixeLenght);
      pthread_create(&g_printThread, NULL, printThreaded, NULL);


      break;
  }
  
  if(g_bufferLenght > 0){
    g_printBuffer = (char *)malloc(g_bufferLenght);
  }
  
  if(g_bufferLenght > (columns + 1) * g_pixeLenght){
    printf("inizialize buffer\n");
    inzialazePrintBuffer(rows, columns);//preplace the \n in the buffer at the right position
  }
  printf("Start in 2 sec\n");
    
  sleep(2);
  printf("\e[?25l");//hide cursor
  printf("\033[H"); //move cursor to top left


  clock_gettime(CLOCK_MONOTONIC, &beginTime);
  // Infinite loop to continuously update the grid (once per frame)
  while (1) {
    struct timespec currentTime;
    //globa frame variables
    //iTime = (float)(clock() - beginTime)/ CLOCKS_PER_SEC;
    //frameDebugTime = clock();

    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    iTime = (currentTime.tv_sec - beginTime.tv_sec) + 
            (currentTime.tv_nsec - beginTime.tv_nsec) / 1e9;
    clock_gettime(CLOCK_MONOTONIC, &frameDebugTime);

    g_angle = (iTime * 0.1) * PI ;
    iMouse = (vec2){iTime * 1.5 ,1.5}; 
    //iMouse = (vec2){0. ,2.}; 
    //g_lightSource = (vec3){sin(iTime * 0.5) * 5. + 1., sin(iTime * 0.5) * 3., cos(iTime * 0.5) * 5. + 1.}; 



    //display frame
    //printf("\033[H"); //move cursor to top left
    display(rows, columns); 

    //sleep(0.01);
    //fflush(stdout);
    //printf("ITime: %f seconds", (float)iTime);
    struct timespec frameEndTime;
    clock_gettime(CLOCK_MONOTONIC, &frameEndTime);
    double frameTime = (frameEndTime.tv_sec - frameDebugTime.tv_sec) + 
                        (frameEndTime.tv_nsec - frameDebugTime.tv_nsec) / 1e9;
    //printf("Time taken: %f seconds\n", frameTime); //display debug info
    //printf("FPS %f \n", 1/frameTime); //display debug info
  }

  return 0;
}
