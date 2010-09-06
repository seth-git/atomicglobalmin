////////////////////////////////////////////////////////////////////////////////
// Purpose: This file reads parameters from the input file.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////


#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef double FLOAT;

const int MATRIX_SIZE = 4;
const FLOAT PIE = 3.14159265359;
const FLOAT PIE_X_2 = 6.28318530718;
const FLOAT PIE_OVER_2 = 1.57079632679;
const FLOAT DEG_TO_RAD = PIE * 2 / 360;
const FLOAT RAD_TO_DEG = 360 / (PIE * 2);

// This structure contains information for a 3D point in space.
// The x and y coordinates correspond to the x and y coordinates
// on a computer screen that is pointed north.  The x coordinate
// becomes larger (more positive) as an object moves to the right
// (east). The y coordinate becomes larger as an object travels
// upward. The z coordinate represents depth on a computer screen and
// becomse more positive as an object travels north.  You might think
// of x as longitude, z as latitude, and y as height above sea level.
// The w coordinate is helpful only in computations and does not
// represent real data.
typedef struct
{
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT w;
} Point3D;

#endif

