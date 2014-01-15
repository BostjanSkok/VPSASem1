/*
 *  vectors.h
 *  RayTracer
 *
 *  Created by Patricio Bulic on 5/30/13.
 *  Copyright 2013 University of Ljubljana, Faculty of Computer and Information Science. All rights reserved.
 *
 */


#ifndef VECTORS_H
#define VECTORS_H

#include "defs.h"

void vec_sub (VECTOR3D *v1, VECTOR3D *v2, VECTOR3D *v3);
void vec_add (VECTOR3D *v1, VECTOR3D *v2, VECTOR3D *v3);
void vec_scale (double scale, VECTOR3D *v1, VECTOR3D *v2);
double dotproduct(VECTOR3D *v1, VECTOR3D *v2);
void normalize_vector (VECTOR3D *v);



#endif

