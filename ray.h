/*
 *  ray.h
 *  RayTracer
 *
 *  Created by Patricio Bulic on 5/30/13.
 *  Copyright 2013 University of Ljubljana, Faculty of Computer and Information Science. All rights reserved.
 *
 */


#ifndef RAY_H
#define RAY_H

#include "defs.h"

void compute_ray(RAY* ray, VECTOR3D* view_point, VIEWPORT* viewport, PIXEL* pixel, VEC_BASIS* camera_frame, double distance);
void compute_shadow_ray(RAY* ray, SPHERE_INTERSECTION* intersection, VECTOR3D* light);
void compute_reflected_ray(RAY* reflected_ray, RAY* incidence_ray, SPHERE_INTERSECTION* intersection);


#endif
