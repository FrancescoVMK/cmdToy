
#include "cmdtoymath.h"
#include "cmdtoyengine.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//shader code

//frag color
vec3 mainImage(vec2 * fragCoord){

  //iResolution vec2
  vec3 fragColor; //out, its vec3 and not vec4 like in shader toy

  // Normalized pixel coordinates (from 0 to 1)
  // vec2 uv = fragCoord/iResolution.xy;
  vec2 uv = vec2_div(*fragCoord, iResolution);


  // Output to screen
  fragColor = (vec3){uv.x, uv.y, abs(sin(iTime))};

  return fragColor;
  
}


int main(int argc, char const *argv[])
{
  cmdToyRender(4, 10, mainImage);
  return 0;
}
