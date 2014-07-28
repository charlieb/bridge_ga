#ifndef VEC3
#define VEC3

typedef struct v3 { 
  float x;
  float y;
  float z;
} v3;

void v3add(const v3 *v1, const v3 *v2, v3 *out);
void v3sub(const v3 *v1, const v3 *v2, v3 *out);
void v3mul(const v3 *v, const float m, v3 *out);
void v3div(const v3 *v, const float m, v3 *out);
float v3mag(const v3 *v);
void v3norm(const v3 *v, v3 *out);
float v3dist(const v3 *v1, const v3 *v2);
int flt_eq(float f1, float f2);

void v3test();

#endif
