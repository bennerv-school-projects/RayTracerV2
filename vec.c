#include "vec.h"
#include <math.h>
#include <stdio.h>

/* Creates a 3D vector */
vec3f vec3(float x, float y, float z){
	vec3f new_vec3f = {x,y,z};
	return new_vec3f;
}

/* Create a 2D  vector */
vec2f vec2(float x, float y) {
	vec2f new_vec2f = {x,y};
	return new_vec2f;
}

/* Returns the dot product of a 2D vector */
float vec2_dot(vec2f vecA, vec2f vecB) {
	float result = vecA.x * vecB.x + vecA.y * vecB.y;
	return result;
}

/* Returns the dot product of a 3D vector */
float vec3_dot(vec3f vecA, vec3f vecB) {
	float result = vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z;
	return result;
}

/* Returns the cross product of two vectors */
vec3f vec3_cross(vec3f vecA, vec3f vecB){
	vec3f result;
	// (a2b3 - a3b2)i + (a3b1-a1b3)j + (a1b2+a2b1)k
	result.x = (vecA.y*vecB.z - vecA.z*vecB.y);
	result.y = (vecA.z*vecB.x - vecA.x*vecB.z);
	result.z = (vecA.x*vecB.y - vecA.y*vecB.x);
	return result;
}

/* Normalizes a vector */
vec3f normalize(vec3f vector) {
	float norm = sqrt(powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2));
	if( norm == 0 ) {
		return vector;
	}
	vec3f new_vec = vec3(vector.x / norm, vector.y / norm, vector.z / norm);
	return new_vec;
}

/* Negates or takes the negative of a vector */
vec3f negate(vec3f vector) {
	return vec3(-vector.x, -vector.y, -vector.z);
}

/* Subtracts two vectors */
vec3f vec3_sub(vec3f vecA, vec3f vecB) {
	return vec3(vecA.x-vecB.x, vecA.y-vecB.y, vecA.z - vecB.z);
}

/* Adds to vectors */
vec3f vec3_add(vec3f vecA, vec3f vecB) {
	return vec3(vecA.x+vecB.x, vecA.y+vecB.y, vecA.z + vecB.z);
}

/* Scalar multiplication of a vector and number */
vec3f scalar_mult(float mult, vec3f vec) {
	return vec3(mult * vec.x, mult * vec.y, mult * vec.z);
}

/* Distance formula between two points */
float distance(vec3f pointA, vec3f pointB) {
	return sqrt(pow(pointB.x - pointA.x, 2) + pow(pointB.y - pointA.y, 2) + pow(pointB.z - pointA.z, 2));
}

/* Reflection vector calculation */
vec3f reflect(vec3f ray, vec3f norm) {
	vec3f ret;
	float temp = vec3_dot(ray, norm);
	temp *= 2;
	ret = normalize(vec3_sub(ray, scalar_mult(temp, norm)));	
	return ret;
}