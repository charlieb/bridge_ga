#include <string.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "vec3.h"

void v3add(const v3 *v1, const v3 *v2, v3 *out) {
  v3 tmp;
  tmp.x = v1->x + v2->x;
  tmp.y = v1->y + v2->y;
  tmp.z = v1->z + v2->z;
  memcpy(out, &tmp, sizeof(v3));
}

void v3sub(const v3 *v1, const v3 *v2, v3 *out) {
  v3 tmp;
  tmp.x = v1->x - v2->x;
  tmp.y = v1->y - v2->y;
  tmp.z = v1->z - v2->z;
  memcpy(out, &tmp, sizeof(v3));
}

void v3mul(const v3 *v, const float m, v3 *out) {
  v3 tmp;
  tmp.x = v->x * m;
  tmp.y = v->y * m;
  tmp.z = v->z * m;
  memcpy(out, &tmp, sizeof(v3));
}
  
void v3div(const v3 *v, const float m, v3 *out) {
  v3 tmp;
  tmp.x = v->x / m;
  tmp.y = v->y / m;
  tmp.z = v->z / m;
  memcpy(out, &tmp, sizeof(v3));
}

float v3mag(const v3 *v) {
  return sqrt(v->x * v->x 
              + v->y * v->y 
              + v->z * v->z);
}

void v3norm(const v3 *v, v3 *out) {
  const float mag = v3mag(v);
  if(0.0f == mag) {
    out->x = 1;
    out->y = out->z = 0;
  }
  else
    v3div(v, mag, out);
}

float v3dist(const v3 *v1, const v3 *v2) {
  v3 d;
  v3sub(v1, v2, &d);
  return v3mag(&d);
}

int flt_eq(float f1, float f2) { return f1 == f2 || (f1 + FLT_MIN >= f2 && f1 - FLT_MIN <= f2); }

void v3test() {
  v3 a,b,c;
  a.x = a.y = a.z = 0.0f;
  b.x = b.y = b.z = 0.0f;
  c.x = c.y = c.z = 0.0f;

  a.x = 1.0f;
  a.y = 2.0f;
  a.z = 3.0f;
  v3add(&a, &a, &a);
  if(a.x == 2 && a.y == 4 && a.z == 6) 
    printf("PASS v3add\n");
  else
    printf("FAIL v3add \n"); 

  v3sub(&a, &a, &a);
  if(a.x == 0 && a.y == 0 && a.z == 0) 
    printf("PASS v3sub\n");
  else
    printf("FAIL v3sub \n"); 

  a.x = 1.0f;
  a.y = 2.0f;
  a.z = 3.0f;
  v3mul(&a, 10.0f, &a);
  if(a.x == 10 && a.y == 20 && a.z == 30) 
    printf("PASS v3mul\n");
  else
    printf("FAIL v3mul \n"); 

  v3div(&a, 10.0f, &a);
  if(a.x == 1 && a.y == 2 && a.z == 3) 
    printf("PASS v3div\n");
  else
    printf("FAIL v3div \n"); 

  a.x = 1.0f;
  a.y = 2.0f;
  a.z = 3.0f;
  if(flt_eq(v3mag(&a), sqrt(1+4+9)))
    printf("PASS v3mag\n");
  else
    printf("FAIL v3mag \n");

  a.x = 1.0f;
  a.y = 2.0f;
  a.z = 3.0f;
  v3norm(&a, &a);
  b.x = 2.0f;
  b.y = 3.0f;
  b.z = 1.0f;
  v3norm(&b, &b);
  if(flt_eq(v3mag(&a), v3mag(&b))) 
    printf("PASS v3norm\n");
  else
    printf("FAIL v3norm \n"); 
}
