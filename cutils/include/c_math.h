#ifndef C_MATH_H
#define C_MATH_H

typedef struct vec2 {
	float x, y;
} vec2;

typedef struct vec2i {
	int x, y;
} vec2i;

typedef struct vec3 {
	float x, y, z;
} vec3;

typedef union vec4 {
	struct {
		float x, y, z, w;
	};
	struct {
		float r, g, b, a;
	};
} vec4;

typedef union mat4 {
	float m[4][4];
	float a[16];
	struct {
		float a11, a12, a13, a14;
		float a21, a22, a23, a24;
		float a31, a32, a33, a34;
		float a41, a42, a43, a44;
	};
} mat4;

typedef struct quaternion {
	float w, x, y, z;
} quaternion;

#endif
