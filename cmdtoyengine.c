#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>
#include <linux/time.h>
#include <string.h>
#include <uchar.h>
#include <pthread.h>
#include <stdint.h>
#include "cmdtoymath.h"
#include "cmdtoyengine.h"

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
float iTime;

// iMouse: Mouse coordinates (x, y), typically normalized or screen-space
vec2 iMouse;

// iResolution: Screen resolution (width, height) in pixels
vec2 iResolution;

//publics
clock_t beginTime ;

static struct winsize w;

static int g_maxThreadNumber = 10;
static int g_thread_rows = 0;
static int g_thread_columns = 0;

static int g_bufferLenght = 0;
static char* g_printBuffer;
static char* g_printBufferToPrint;
pthread_t g_printThread;

static vec3 (*g_mainImage)(vec2 * fragCoord);




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
  vec3 color = g_mainImage(&uv);

  return colorToColoredChar(&color);
}

//same as get but you pass the buffer (already offsetted of the position u need)
char *setPixel(int x,int y, char* buf){

  vec2 uv = (vec2){(float)x, (float)y};
  vec3 color = g_mainImage(&uv);

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



void* printThreaded(void* args) {
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


int cmdToyRender(int mode, int maxThreadNumber, vec3 (*mainImage)(vec2 * fragCoord)) {
    
  g_mainImage = mainImage;
  

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
  void (*display)(int, int);
  

  switch(mode){
    //allocate memory for print buffer if used (+1 column for the \n) + 1 for end char + 1 fpr end \n
    case 0: //char mode
      printf("0 display char mode\n");
      display = displayGridChar;
      break;
    case 1: //line mode
      printf("1 display line mode\n");
      g_bufferLenght = columns * g_pixeLenght + columns + 1; 
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

      
	  g_maxThreadNumber = maxThreadNumber;
      
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

    //display frame
    //printf("\033[H"); //move cursor to top left

    display(rows, columns); 

    //sleep(0.01);
    //fflush(stdout);
    //printf("ITime: %f seconds", (float)clock());
    struct timespec frameEndTime;
    clock_gettime(CLOCK_MONOTONIC, &frameEndTime);
    double frameTime = (frameEndTime.tv_sec - frameDebugTime.tv_sec) + 
                        (frameEndTime.tv_nsec - frameDebugTime.tv_nsec) / 1e9;
    //printf("Time taken: %f seconds\n", frameTime); //display debug info
    //printf("FPS %f \n", 1/frameTime); //display debug info
  }

  return 0;
}
