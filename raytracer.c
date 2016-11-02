/* Author: Sevin Dennis, Ben Vesel
 * Program: RayTracer
 * Course: CS4611
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "geom.h"
#include "vec.h"

int debug = 0;

/* Ray structure */
typedef struct {
   vec3f vector; // Ray vector ( should be normalized )
   vec3f posn; // starting position of vector
} Ray;

/* RayHit structure */
typedef struct {
	Ray ray;  // The original ray that intersected with the object
	float time; // The time/distance where the intersection occurred
	vec3f normal; // The normal vector of the intersection
	int reflective; // Whether or not the intersection was with a reflective object
	vec3f hitPoint; // The point on the cube/triangle where the ray hit
	vec3f color; // The color of the pixel at this ray hit portion
} RayHit;

/* Perspective matrix structure */
typedef struct {
	vec3f light_pos; // The position of the light source
  	vec3f camera_pos; // The camera position
  	int distance; // Distance to the draw plane
  	int screen_width_world; // Screen width in units (meters)
  	int screen_width_pixels; // Screen width in pixels
  	float units_per_pixel; // Units per pixel 
} Perspective;

/* Object information */
material refl;
material red;
material blue;
material white;
material green;

sphere sph1;
sphere sph2;
sphere sph3;
triangle back1;
triangle back2;
triangle bot1;
triangle bot2;
triangle right;

/* Gets the color of a pixel located at the given coordinate */
vec3f get_pixel_color(vec2f cord, unsigned char * array, int width){

	int pos = (cord.y*3*width)+(cord.x*3);
	return vec3(array[pos],array[pos+1],array[pos+2]);
}

/* Sets the color of a pixel given a 2d coordinate */
void set_pixel_color(vec3f color, vec2f cord, unsigned char * array, int width){

	int pos = (cord.y*3*width)+(cord.x*3);

	array[pos] = color.x; //R
	array[pos+1] = color.y; //G
	array[pos+2] = color.z; //B
}

/* Sets up the geometry for the reference portion */
void referenceGeometry() {
	
	// create three spheres
	sphere_new(vec3(0,0,-16), 2, &sph1, refl);
	sphere_new(vec3(3,-1,-14), 1, &sph2, refl);
	sphere_new(vec3(-3,-1,-14), 1, &sph3, red);

	// back wall
	triangle_new(vec3(-8,-2,-20),
	vec3(8,-2,-20),
	vec3(8,10,-20), &back1,blue);

	triangle_new(vec3(-8,-2,-20),
		vec3(8,10,-20),
		vec3(-8,10,-20), &back2,blue);

	// floor	
	triangle_new(vec3(-8,-2,-20),
		vec3(8,-2,-10),
		vec3(8,-2,-20), &bot1,white);
	
	triangle_new(vec3(-8,-2,-20),
		vec3(-8,-2,-10),
		vec3(8,-2,-10), &bot2,white);
		
	// right red triangle
	triangle_new(vec3(8,-2,-20),
		vec3(8,-2,-10),
		vec3(8,10,-20), &right,red);

}

void nonReferenceGeometry() {
	
	// create three spheres
	sphere_new(vec3(0,0,8), 2, &sph1, green);
	sphere_new(vec3(3,-1,7), 1, &sph2, refl);
	sphere_new(vec3(-3,-1,7), 1, &sph3, red);

	// back wall
	triangle_new(vec3(-8,-2,-10),
	vec3(8,-2,-10),
	vec3(8,10,-10), &back1,refl);

	triangle_new(vec3(-8,-2,-10),
		vec3(8,10,-10),
		vec3(-8,10,-10), &back2,refl);

	// floor	
	triangle_new(vec3(-8,-2,10),
		vec3(8,-2,5),
		vec3(8,-2,10), &bot1,white);
	
	triangle_new(vec3(-8,-2,10),
		vec3(-8,-2,5),
		vec3(8,-2,5), &bot2,white);
		
	// right red triangle
	triangle_new(vec3(8,-2,10),
		vec3(8,-2,5),
		vec3(8,10,10), &right,red);
	
}

/* Get a normal to a sphere give a sphere index and the point on the sphere */
vec3f normalSphere( vec3f coords, int sphereIndex ) {
	if( sphereIndex > sphere_index) {
		return vec3(0, 0, 0);
	}
	vec3f center = sph[sphereIndex].pos;
	vec3f result = normalize(vec3_sub(coords, center));
	return result;
}

/* get a normal to a triangle given a triangle index */
vec3f normalTriangle( int triangleIndex ) {
	if( triangleIndex > triangle_index ) {
		return vec3(0, 0, 0);
	}
	
	vec3f vec1 = vec3_sub( tri[triangleIndex].posB, tri[triangleIndex].posA );
	vec3f vec2 = vec3_sub( tri[triangleIndex].posB, tri[triangleIndex].posC );
	return normalize(vec3_cross(vec2, vec1));

}

/* Checks for intersection between a triangle and a ray */
int sphere_intersect(RayHit * rayHit) {

	int retVal = 0;
	float time0 = -1;
	float time1 = -1;
	float trueTime;
	rayHit->color = vec3(0, 0, 0);


	for( int i = 0; i < sphere_index; i++) {
		float front = vec3_dot(negate(rayHit->ray.vector), vec3_sub(rayHit->ray.posn, sph[i].pos));
		float second = pow(vec3_dot(rayHit->ray.vector, vec3_sub(rayHit->ray.posn, sph[i].pos)), 2) - (vec3_dot(rayHit->ray.vector, rayHit->ray.vector) * (vec3_dot(vec3_sub(rayHit->ray.posn, sph[i].pos), vec3_sub(rayHit->ray.posn, sph[i].pos)) - pow(sph[i].radius, 2)));

		if( second < 0 ) {
			//printf( "no solution" );
		} else {
			second = sqrt(second);
			time0 = (front + second) / vec3_dot(rayHit->ray.vector, rayHit->ray.vector);
			time1 = (front - second) / vec3_dot(rayHit->ray.vector, rayHit->ray.vector);
			if( time0 < 0 && time1 < 0 ) {
				return retVal;
				if( debug ) {
					printf("BAD VALUES\n");
				}
			}
			
			trueTime = time0 < time1 ? time0 : time1;
			if( trueTime < rayHit->time || rayHit->time < 0 ) {
				rayHit->time = trueTime;
				rayHit->color = sph[i].mat.color;
				rayHit->reflective = sph[i].mat.reflective;
				rayHit->hitPoint = vec3_add(rayHit->ray.posn, scalar_mult(rayHit->time, rayHit->ray.vector));
				rayHit->normal = normalSphere( rayHit->hitPoint, i );
				retVal = 1;
			}
		}	
	}
	return retVal;
}

/* Checks for intersection between a triangle and a ray */
int triangle_intersect(RayHit * rayHit) {
	float A, B, C, D, E, F, G, H, I, J, K, L, M, beta, gamma, time0;
	int retVal = 0;
	for( int i = 0; i < triangle_index; i++) {
		triangle temp = tri[i];
		A = temp.posA.x - temp.posB.x;
		B = temp.posA.y - temp.posB.y;
		C = temp.posA.z - temp.posB.z;
		D = temp.posA.x - temp.posC.x;
		E = temp.posA.y - temp.posC.y;
		F = temp.posA.z - temp.posC.z;
		G = rayHit->ray.vector.x;
		H = rayHit->ray.vector.y;
		I = rayHit->ray.vector.z;
		J = temp.posA.x - rayHit->ray.posn.x;
		K = temp.posA.y - rayHit->ray.posn.y;
		L = temp.posA.z - rayHit->ray.posn.z;
		
		
		
		
		M = A * ( E * I - H * F) + B * ( G * F - D * I ) + C * ( D * H - E * G );	
		time0 = (-1 * (F * (A * K - J * B) + E * (J * C - A * L) + D * ( B * L - K * C) )) / M;
		if( time0 < 0  ) {
			continue;
		}
		gamma = ( I * ( A * K - J * B ) + H * ( J * C - A * L ) + G * (B * L - K * C )) / M;
		if( gamma < 0 || gamma > 1 ) {
			continue;
		}
		beta = ( J * ( E * I - H * F ) + K * ( G * F - D * I ) + L * ( D * H - E * G ) ) / M;
		if( beta < 0 || beta > 1 - gamma ) {
			continue;
		}
		
		//RayHit intersection and time is the least
		if( rayHit->time > time0 ) {
			rayHit->color = tri[i].mat.color;
			rayHit->time = time0;
			rayHit->reflective = tri[i].mat.reflective;
			rayHit->hitPoint = vec3_add(rayHit->ray.posn, scalar_mult(rayHit->time, rayHit->ray.vector));
			rayHit->normal = normalTriangle(i);
			retVal = 1;

		}
	}
	return retVal;
}

/* Returns a Ray to use with something. */
void GetInitialRay(Perspective p, vec2f screenCoord, Ray *newRay){

	// Calculate the exact location of the middle of the pixel (offset of .5 * units_per_pixel)
	newRay->vector = normalize(vec3((-1 + screenCoord.x*p.units_per_pixel + .5 * p.units_per_pixel), (1 - screenCoord.y * p.units_per_pixel - .5 * p.units_per_pixel), -2));

}

/* Returns a number corresponding with the dot product of the light vector and the normal */
float CheckShadows( Perspective per, RayHit * rayHit ) {
	Ray tempRay;
	memset(&tempRay, 0, sizeof(Ray));
	
	// Add a 'bump' to the hit/intersection point
	vec3f normal = rayHit->normal;
	vec3f newHitPoint = vec3( normal.x * 0.00005 + rayHit->hitPoint.x, 
							  normal.y * 0.00005 + rayHit->hitPoint.y, 
							  normal.z * 0.00005 + rayHit->hitPoint.z );
	tempRay.posn = newHitPoint;
	
	// Find a ray from the point given to the direction of the light
	tempRay.vector = vec3_sub(per.light_pos, newHitPoint);
	tempRay.vector = normalize(tempRay.vector);
	
	//Check if that ray intersects with any object
	RayHit newRayHit;
	newRayHit.ray = tempRay;
	newRayHit.time = FLT_MAX;
	
	
	int intersect = sphere_intersect(&newRayHit);
	intersect |= triangle_intersect(&newRayHit);
	
	// There was an intersection so we're in a shadow
	if( intersect ) {
		return 0.2;
	} 
	
	// Ambient lighting scale
	float temp = vec3_dot(tempRay.vector, rayHit->normal);
	if( temp < 0.2 ) {
		temp = 0.2;
	}
	
	return temp;	
}

/* get the reflection color */
void CheckReflection(RayHit * rayHit, int depth) {
	// Base case color = black
	if( depth == 10 ) {
		rayHit->color = vec3(0, 0, 0);
		return;
	}
	
	// Get our reflection vector (and add in a 'bump')
	rayHit->ray.vector = reflect(rayHit->ray.vector, rayHit->normal);
	rayHit->ray.posn = vec3( rayHit->normal.x * 0.00005 + rayHit->hitPoint.x, 
							 rayHit->normal.y * 0.00005 + rayHit->hitPoint.y,
							 rayHit->normal.z * 0.00005 + rayHit->hitPoint.z );
	rayHit->time = FLT_MAX;

	
	int intersect = sphere_intersect(rayHit);
	if( debug ) {
		printf("sphere intersection is %d\n", intersect);
	}
	intersect |= triangle_intersect(rayHit);
	
	// Continue recursion if we hit a reflective surface
	if( intersect && rayHit->reflective ) {
		CheckReflection( rayHit, depth + 1 );
	} else {
		return;
	}
}

/*Main of the Program.*/
int main(int argc, char *argv[]){

	int width = 512;
	int height = 512;
	char * imageFileName;
	int isReference = 0;
	int shouldFree = 0;

	//Set the colors
	// make a material which is reflective
	refl.reflective = 1;
	refl.color = vec3(0,0,0); // color is not used when material is reflective!

	// make several diffuse materials to choose from
	red.reflective = 0;
	red.color = vec3(255,0,0);

	blue.reflective = 0;
	blue.color = vec3(0,0,255);

	white.reflective = 0;
	white.color = vec3(255,255,255);
	
	green.reflective = 0;
	green.color = vec3(0, 255, 0);
	
	// Not enough command line arguments
	if( argc < 2 ) {
		printf("Did not specify enough arguments.  Using reference.png\n");
		imageFileName = "reference.png";
		isReference = 1;
		
	// Handle file name command line arg
	} else {
		
		//Allocate memory for the file name
		imageFileName = malloc(sizeof(char) * 50);
		shouldFree = 1;
		if( !imageFileName ) {
			printf("failed to allocate memory.  Exiting\n");
			exit(9);
		}
		strncpy(imageFileName, argv[1], 49 );
		int offset = strlen(imageFileName);
		if(strncmp("reference", imageFileName, 10) == 0) {
			isReference = 1;
		}
		
		// Offset for adding in the .png extension
		if( offset + 3 > 50 ) {
			printf("Specified a file name too long.  Exiting\n");
		}
		
		//Filename extension
		imageFileName[offset] = '.';
		imageFileName[offset+1] = 'p';
		imageFileName[offset+2] = 'n';
		imageFileName[offset+3] = 'g';
	}
	
	//Generate the array for the Image
	unsigned long size = height * width * 3 * sizeof(unsigned char);
	unsigned char * arrayContainingImage = malloc(size);
	
	if( !arrayContainingImage ) {
		printf("Failed to allocate memory.  Exiting\n");
		exit(10);
	}
	
	//Our camera pos and perspective
	Perspective myPer;
	myPer.light_pos = vec3( 3, 5, -15 ); // light position
	myPer.camera_pos = vec3(0,0,0); // Camera Pos
	myPer.distance = -2; // 2 units from screen...
	myPer.screen_width_world = 2; //Screen width in meters
	myPer.screen_width_pixels = width; //Screen width in pixels
	myPer.units_per_pixel = ((float) myPer.screen_width_world)/myPer.screen_width_pixels;

	// Set up the reference geometry
	if( isReference ) {
		referenceGeometry();
	} else {
		nonReferenceGeometry();
		myPer.light_pos = vec3(-3, 5, 5);
	}

	// Go through all the pixels on the screen
	for(int x = 0;x<height;x++){
		for(int y=0; y<width;y++){
			//create a ray
			Ray myRay;
			myRay.posn = vec3(0, 0, 0); // Starting position of vector

			if( x > 200 && x < 230 && y > 250 && y < 290 ) {
				debug = 1;
			} else {
				debug = 0;
			}
			
			//generate that ray
			GetInitialRay(myPer, vec2(x, y), &myRay);
			
			// Check for intersection
			RayHit rayHit;
			memset(&rayHit, 0, sizeof(RayHit));
			rayHit.time = FLT_MAX;
			rayHit.ray = myRay;
			sphere_intersect(&rayHit);
			triangle_intersect(&rayHit);
			
			// Scale the diffuse shading
			if( rayHit.reflective ) {
				CheckReflection(&rayHit, 0);
			}

			float scale = CheckShadows(myPer, &rayHit );
			//printf( "scale is %f\n", scale);
			rayHit.color.x *= scale;
			rayHit.color.y *= scale;
			rayHit.color.z *= scale;			

			// Color the pixel by the right object
			set_pixel_color(rayHit.color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
					
		}
	}
	
	//Write out Image
	stbi_write_png(imageFileName, width, height, 3, arrayContainingImage, width*3);
	free(arrayContainingImage);
	if( shouldFree ) 
		free(imageFileName);

}




