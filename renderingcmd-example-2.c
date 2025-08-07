
#include "cmdtoymath.h"
#include "cmdtoyengine.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

vec3 mainImage(vec2 * fragCoord){
  vec3 fragColor; //out, its vec3 and not vec4 like in shader toy
  fragColor = (vec3){1.000,1.000,1.000};
  float aspectRatio = iResolution.x / iResolution.y;
  //printf("%f", iResolution.x);
  // Normalized pixel coordinates (from 0 to 1)
  vec2 fragCordScaled = vec2_scale(*fragCoord, 2.0);
  fragCordScaled.x = fragCordScaled.x / iResolution.x;
  fragCordScaled.y = fragCordScaled.y / iResolution.y;

  vec2 uv = (vec2){fragCordScaled.x - 1., fragCordScaled.y - 1.};

  // Correct for aspect ratio
  uv.x = uv.x * aspectRatio;
  

  
  vec2_scale_p(&uv, sin(iTime) * 50.);

  uv.x = floor(uv.x);
  uv.y = floor(uv.y);
  if(fmod(uv.x + uv.y, 2.) == 0.){

      if(sin(iTime + uv.x) + cos(iTime + uv.y) > sin(iTime))
      fragColor = (vec3){.000,.000,.000};
  }else{
      if(fmod(uv.y, 2.) != 0.)
      fragColor = (vec3){.000,.000,.000};
  }


  return fragColor;
}



int main(int argc, char const *argv[])
{
  cmdToyRender(4, 10, mainImage);
  return 0;
}
