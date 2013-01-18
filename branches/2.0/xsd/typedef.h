////////////////////////////////////////////////////////////////////////////////
// Purpose: This file reads parameters from the input file.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////


#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include <cstring>

#define PRINT_CATCH_MESSAGES true

typedef double FLOAT;

static const unsigned int MATRIX_WIDTH = 4;
typedef FLOAT COORDINATE3[3];
typedef FLOAT COORDINATE4[MATRIX_WIDTH];
static const size_t MATRIX_SIZE = MATRIX_WIDTH*MATRIX_WIDTH;
static const FLOAT IDENTITY_MATRIX[MATRIX_WIDTH][MATRIX_WIDTH] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
};
static const FLOAT PIE = 3.14159265359;
static const FLOAT PIE_X_2 = 6.28318530718;
static const FLOAT PIE_OVER_2 = 1.57079632679;
static const FLOAT DEG_TO_RAD = PIE_X_2 / 360;
static const FLOAT RAD_TO_DEG = 360 / PIE_X_2;
static const unsigned int MAX_ATOMIC_NUMBERS = 118;

#endif
