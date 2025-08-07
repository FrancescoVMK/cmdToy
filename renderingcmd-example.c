
#include "cmdtoymath.h"
#include "cmdtoyengine.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
  /*
  float t = 0.1;
  Point minDistP = (Point){maxDist, g_backGroundColor, -1};
  for( int i=0; i<256 && t<maxDist; i++ ){
    Point h = map(vec3_add(ro, vec3_scale(rd, t)), currentShapeId);
    if(h.dist < g_min_dist)
      return h;
             
        
    t += h.dist;
  }
    
  return minDistP;
  */

  int jShapes = -1;
    
  Point p;
  float dist = 0.1;
  Point oldP = (Point){g_max_dist, g_backGroundColor, -1};
  float firstDist;
  


 
  for( int i=0; i<256 && dist<maxDist; i++ ){
    p = map(vec3_add(ro, vec3_scale(rd, dist)), jShapes);
    if(p.dist <= g_min_dist){
      if(p.color.k >= 1.0){ 
        return p;

      }else{
        oldP.color = vec4_mix(oldP.color, p.color, p.color.k);
        if(oldP.dist >= g_max_dist){
          oldP.dist = p.dist;  
          //printf("| %f %f |",firstDist, p.dist);       
          oldP.shapeId = p.shapeId;
        }
        jShapes = p.shapeId;
      }
    }

    dist += p.dist;
  }
  
  return oldP;
  

}


void camera(vec2* uv, vec3* ro, vec3* rd){  //out vec3 ro, out vec3 rd
  //camera from https://www.shadertoy.com/view/XsXXDB
  static const vec3 zoom = (vec3){0.0, 6.0, 0.0};
  static const vec3 ta = (vec3){0.0, 0.0, 0.0};
  static const vec3 cp = (vec3){0.0, 1.0, 0.0 };
    
  vec2 m = vec2_div(vec2_add(iMouse, (vec2){iTime, 0.}) , iResolution);

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

      color = vec3_mix(
        vec3_scale((vec3){pointShadow.color.x, pointShadow.color.y, pointShadow.color.z}, 1.), 
        vec3_scale(color, pointShadow.dist),  
        
        pointShadow.color.k);

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
  g_angle = sin(iTime);
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




int main(int argc, char const *argv[])
{
  cmdToyRender(4, 10, mainImage);
  return 0;
}

