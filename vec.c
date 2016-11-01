#include "vec.h"
#include <math.h>
#include <stdio.h>


vec3f vec3(float x, float y, float z){
	vec3f new_vec3f = {x,y,z};
	return new_vec3f;
}

vec2f vec2(float x, float y) {
	vec2f new_vec2f = {x,y};
	return new_vec2f;
}

float vec2_dot(vec2f vecA, vec2f vecB) {
	float result = vecA.x * vecB.x + vecA.y * vecB.y;
	return result;
}

float vec3_dot(vec3f vecA, vec3f vecB) {
	float result = vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z;
	return result;
}
// A cross B
vec3f vec3_cross(vec3f vecA, vec3f vecB){
	vec3f result;
	// (a2b3 - a3b2)i + (a3b1-a1b3)j + (a1b2+a2b1)k
	result.x = (vecA.y*vecB.z - vecA.z*vecB.y);
	result.y = (vecA.z*vecB.x - vecA.x*vecB.z);
	result.z = (vecA.x*vecB.y - vecA.y*vecB.x);
	return result;
}

vec3f normalize(vec3f vector) {
	float norm = sqrt(powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2));
	if( norm == 0 ) {
		printf("ERROR\n");
		return vector;
	}
	vec3f new_vec = vec3(vector.x / norm, vector.y / norm, vector.z / norm);
	return new_vec;
}

vec3f negate(vec3f vector) {
	return vec3(-vector.x, -vector.y, -vector.z);
}

vec3f vec3_sub(vec3f vecA, vec3f vecB) {
	return vec3(vecA.x-vecB.x, vecA.y-vecB.y, vecA.z - vecB.z);
}

vec3f vec3_add(vec3f vecA, vec3f vecB) {
	return vec3(vecA.x+vecB.x, vecA.y+vecB.y, vecA.z + vecB.z);
}

vec3f scalar_mult(float mult, vec3f vec) {
	return vec3(mult * vec.x, mult * vec.y, mult * vec.z);
}

float distance(vec3f pointA, vec3f pointB) {
	return sqrt(pow(pointB.x - pointA.x, 2) + pow(pointB.y - pointA.y, 2) + pow(pointB.z - pointA.z, 2));
}