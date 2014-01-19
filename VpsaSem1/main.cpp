/*
*  main.c
*  RayTracer
*
*  Created by Patricio Bulic on 5/30/13.
*  Copyright 2013 University of Ljubljana, Faculty of Computer and Information Science. All rights reserved.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include "defs.h"
#include "vectors.h"
#include "sphere.h"
#include "shading.h"
#include "ray.h"
#include <iomanip>

#include <GL/gl.h>
#include "glut.h"

#include <omp.h>

#define NSPHERES 4
#define VIEWPLANE 400
#define WINDOW VIEWPLANE*2
#define FOCALDIST 1000
#define RADIUS 200

VEC_BASIS camera_frame;
VECTOR3D view_point;
VECTOR3D light;
SPHERE sphere[NSPHERES];
VIEWPORT viewport;
/*RAY ray, shadow_ray, reflected_ray;
PIXEL pixel;
SPHERE_INTERSECTION intersection, current_intersection, shadow_ray_intersection, reflected_ray_intersection, current_reflected_intersection;*/

double focal_distance;
double color;
//double kd_red, kd_green, kd_blue;
//double ks_red, ks_green, ks_blue;
//double red, green, blue;
double light_intensity, ambi_light_intensity;
//double theta, reflected_theta;

//BOOLEAN bShadow;

DIRECTION direction[NSPHERES];

/*int intersection_object = -1; // none
int reflected_intersection_object = -1; // none
double current_lambda = 0x7fefffffffffffff; // maximum positive double
double current_reflected_lambda = 0x7fefffffffffffff; // maximum positive double*/


// window identifier:
static int win;
using namespace std;
void Timer (int obsolete) {

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 0);
}

void init(void) {

	direction[0] = up;
	direction[1] = down;
	direction[2] = up;

	/*pixel.i = 0;
	pixel.j = 0;*/

	// set scene:
	viewport.xvmin = -VIEWPLANE;
	viewport.yvmin = -VIEWPLANE;
	viewport.xvmax = VIEWPLANE;
	viewport.yvmax = VIEWPLANE;

	camera_frame.u.x = 1.0;
	camera_frame.u.y = 0.0;
	camera_frame.u.z = 0.0;

	camera_frame.v.x = 0.0;
	camera_frame.v.y = 1.0;
	camera_frame.v.z = 0.0;

	camera_frame.n.x = 0.0;
	camera_frame.n.y = 0.0;
	camera_frame.n.z = 1.0;

	view_point.x = (viewport.xvmax - viewport.xvmin) / 2.0 ;
	view_point.y = (viewport.yvmax - viewport.yvmin) / 2.0 ;
	view_point.z = 0.0;


	light.x = view_point.x - 1300;
	light.y = view_point.y + 1300 ;
	light.z = view_point.z - 300;


	ambi_light_intensity = 1.0;
	light_intensity = 1.0;

	focal_distance = FOCALDIST;


	sphere[0].radius = RADIUS/1.5;
	sphere[0].center.x  = view_point.x - (RADIUS+30);
	sphere[0].center.y  = view_point.y ;
	sphere[0].center.z  = view_point.z - focal_distance - (2*RADIUS+20);
	// the first sphere is blue:
	set_rgb_array(sphere[0].kd_rgb, 0.0, 0.0, 0.8);
	set_rgb_array(sphere[0].ks_rgb, 1.0, 1.0, 1.0);
	set_rgb_array(sphere[0].ka_rgb, 0.0, 0.0, 0.2);
	sphere[0].shininess = 100.0;
	sphere[0].mirror = false;

	sphere[1].radius = RADIUS/1.2;
	sphere[1].center.x  = view_point.x + 0;
	sphere[1].center.y  = view_point.y + 50;
	sphere[1].center.z  = view_point.z - focal_distance - (3*RADIUS+20);
	// the second sphere is green:
	set_rgb_array(sphere[1].kd_rgb, 0.0, 0.5, 0.0);
	set_rgb_array(sphere[1].ks_rgb, 1.0, 1.0, 1.0);
	set_rgb_array(sphere[1].ka_rgb, 0.0, 0.2, 0.0);
	sphere[1].shininess = 10.0;
	sphere[1].mirror = false;


	sphere[2].radius = RADIUS;
	sphere[2].center.x  = view_point.x + (2*RADIUS+30);
	sphere[2].center.y  = view_point.y + 100;
	sphere[2].center.z  = view_point.z - focal_distance - (4*RADIUS+20);
	// the third sphere is red:
	set_rgb_array(sphere[2].kd_rgb, 1.0, 0.0, 0.0);
	set_rgb_array(sphere[2].ks_rgb, 1.0, 1.0, 1.0);
	set_rgb_array(sphere[2].ka_rgb, 0.2, 0.0, 0.0);
	sphere[2].shininess = 100.0;
	sphere[2].mirror = false;


	sphere[3].radius = 100*RADIUS;
	sphere[3].center.x  = view_point.x ;
	sphere[3].center.y  = view_point.y - 100*RADIUS-130;
	sphere[3].center.z  = view_point.z - focal_distance - (4*RADIUS+20);
	// the third sphere is red:
	set_rgb_array(sphere[3].kd_rgb, 0.5, 0.5, 0.5);
	set_rgb_array(sphere[3].ks_rgb, 1.0, 1.0, 1.0);
	set_rgb_array(sphere[3].ka_rgb, 0.5, 0.5, 0.5);
	sphere[3].shininess = 100.0;
	sphere[3].mirror = true;

	// set clearing (background) color to white:
	glClearColor(0.0, 0.0, 0.0, 0.0);
	// specify that ortgogonal 2D projection is to be used to 
	// map context of 2D world coordinats to the screen. We use the 
	// world-coordinate rectangle of the same aspect ratio as the display window
	// so ther is no distortion:
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, WINDOW, 0.0, WINDOW);
}

void animate(void){


	// bounce the first ball:
	if (direction[0] == up) {
		if (sphere[0].center.y < viewport.yvmax+300) {
			sphere[0].center.y +=10;
		}
		else {
			direction[0] = down;
		}
	}
	else if (direction[0] == down) {
		if (sphere[0].center.y > viewport.yvmin+900) {
			sphere[0].center.y -=10;
		}
		else {
			direction[0] = up;
		}
	}

	// bounce the second ball:
	if (direction[1] == up) {
		if (sphere[1].center.y < viewport.yvmax+300) {
			sphere[1].center.y +=20;
		}
		else {
			direction[1] = down;
		}
	}
	else if (direction[1] == down) {
		if (sphere[1].center.y > viewport.yvmin+900) {
			sphere[1].center.y -=20;
		}
		else {
			direction[1] = up;
		}
	}

	// bounce the third ball:
	if (direction[2] == up) {
		if (sphere[2].center.y < viewport.yvmax+300) {
			sphere[2].center.y +=55;
		}
		else {
			direction[2] = down;
		}
	}
	else if (direction[2] == down) {
		if (sphere[2].center.y > viewport.yvmin+900) {
			sphere[2].center.y -=55;
		}
		else {
			direction[2] = up;
		}
	}



	//light.z -= 100;

}

int FrameCount = 0;
void disp(void){

	//int i,j,k,l,m;
	FrameCount++;
	animate();

	//clear all pixels:
	glClear(GL_COLOR_BUFFER_BIT);
	//lastTime = omp_get_wtime();
	//cout <<"l" <<  omp_get_wtime() <<" \n"; //<< setprecision(4)c
	//cout  << FrameCount << ";" << "Code;" << (lastTime - omp_get_wtime()) <<" \n"; //<< setprecision(4)c
	//lastTime = omp_get_wtime();
	double dispTime =  omp_get_wtime();

	// RAY TRACING:
	for (int i=0; i<(viewport.xvmax - viewport.xvmin); i++){

		for (int j=0; j<(viewport.yvmax - viewport.yvmin); j++){
			int intersection_object = -1; // none
			int reflected_intersection_object = -1; // none
			double current_lambda = 0x7fefffffffffffff; // maximum positive double
			double current_reflected_lambda = 0x7fefffffffffffff; // maximum positive double

			RAY ray, shadow_ray, reflected_ray;
			PIXEL pixel;
			SPHERE_INTERSECTION intersection, current_intersection, shadow_ray_intersection, reflected_ray_intersection, current_reflected_intersection;

			double red, green, blue;
			double theta, reflected_theta;

			bool bShadow = false;

			pixel.i = i;
			pixel.j = j;

			// 1. compute ray:
			compute_ray(&ray, &view_point, &viewport, &pixel, &camera_frame, focal_distance);

			double lastTime = omp_get_wtime();
			// 2. check if ray hits an object:
			for (int k=0; k<NSPHERES; k++)
			{
				if (sphere_intersection(&ray, &sphere[k], &intersection))
				{
					// there is an intersection between ray and object
					// 1. Izracunanaj normalu...
					intersection_normal(&sphere[k], &intersection, &ray);
					// 2. ako je lambda presjecista manji od trenutacnog:
					if (intersection.lambda_in<current_lambda)
					{
						current_lambda=intersection.lambda_in;
						intersection_object=k;
						copy_intersection_struct(&current_intersection, &intersection);
					}
					// izracunaj current lambda current_lambda =
					// oznaci koji je trenutacni object : intersection_object = 
					// kopiraj strukturu presjeka : copy_intersection_struct();
				}
			}
			cout  << FrameCount << ";" << "RayObjectColision;" << (lastTime - omp_get_wtime()) <<" \n"; //<< setprecision(4)c


			cout  << FrameCount << ";" << "DispTimeBeforeColorAndOpenGl;" << (dispTime - omp_get_wtime()) <<" \n"; //<< setprecision(4)c

			// Compute the color of the pixel:
			if (intersection_object > -1)
			{
				compute_shadow_ray(&shadow_ray, &intersection, &light);
				theta = dotproduct(&(shadow_ray.direction), &(intersection.normal));

				lastTime = omp_get_wtime();


				for (int l=0; l<NSPHERES; l++)
				{
					if (l!=intersection_object)
					{
						if (sphere_intersection(&shadow_ray, &sphere[l], &shadow_ray_intersection) && (theta>0.0))
							bShadow=true;
					}
				}

				cout  << FrameCount << ";" << "PixelColor;" << (lastTime - omp_get_wtime()) <<" \n"; //<< setprecision(4)c


				if (bShadow)
				{	// if in shadow, add only ambiental light to the surface color
					red = shadow(sphere[intersection_object].ka_rgb[CRED], ambi_light_intensity);
					green = shadow(sphere[intersection_object].ka_rgb[CGREEN], ambi_light_intensity);
					blue = shadow(sphere[intersection_object].ka_rgb[CBLUE], ambi_light_intensity);
				} 
				else
				{
					// the intersection is not in shadow:
					red = blinnphong_shading(&current_intersection, &light, &view_point,
						sphere[intersection_object].kd_rgb[CRED], sphere[intersection_object].ks_rgb[CRED], sphere[intersection_object].ka_rgb[CRED], sphere[intersection_object].shininess,
						light_intensity, ambi_light_intensity);
					green = blinnphong_shading(&current_intersection, &light, &view_point,
						sphere[intersection_object].kd_rgb[CGREEN], sphere[intersection_object].ks_rgb[CGREEN], sphere[intersection_object].ka_rgb[CGREEN], sphere[intersection_object].shininess,
						light_intensity, ambi_light_intensity);
					blue = blinnphong_shading(&current_intersection, &light, &view_point,
						sphere[intersection_object].kd_rgb[CBLUE], sphere[intersection_object].ks_rgb[CBLUE], sphere[intersection_object].ka_rgb[CBLUE], sphere[intersection_object].shininess,
						light_intensity, ambi_light_intensity);
				}
				glColor3f(red, green, blue);
				glBegin(GL_POINTS);
				glVertex2i(i, j);
				glEnd();
				intersection_object = -1;
				bShadow = false;
			}
			else
			{
				// draw the pixel with the background color 
				glColor3f(0.0, 0.0, 0.0);
				glBegin(GL_POINTS);
				glVertex2i(i, j);
				glEnd();
				intersection_object = -1;
				bShadow = false;
			}
			current_lambda = 0x7fefffffffffffff;
			current_reflected_lambda = 0x7fefffffffffffff;
		}
	}
	cout  << FrameCount << ";" << "DispTime;" << (dispTime - omp_get_wtime()) <<" \n"; //<< setprecision(4)c
	//glFlush();
	glutSwapBuffers();
}


int main (int argc, char** argv) {
	freopen( "C:\\Users\\Bostjan\\Documents\\file.csv", "w", stdout );
	cout << "Frame;CodeSection;Time\n";
	glutInit (&argc, argv);
	// specify the display mode to be RGB and single buffering:
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	// specify the initial window position:
	glutInitWindowPosition(100,100);
	// specify the initial window size:
	glutInitWindowSize(WINDOW,WINDOW);
	// create the window and set title:
	win = glutCreateWindow("Basic Ray Tracer by Pa3cio, UL FRI");
	// init opengl:
	init();
	// register callback function to display graphics:
	glutDisplayFunc(disp);
	// call Timer():
	Timer(0);
	// enter tha main loop and process events:
	glutMainLoop();

	return 0;
}


