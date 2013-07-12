/*
 * matrix.h
 *
 *  Created on: Dec 24, 2012
 *      Author: sethcall
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include "../xsd/typedef.h"
#include <string.h>
#include <stdio.h>

static const size_t MATRIX_SIZE = MATRIX_WIDTH * MATRIX_WIDTH;
static const size_t SIZEOF_MATRIX = MATRIX_SIZE * sizeof(FLOAT);

static const FLOAT IDENTITY_MATRIX[MATRIX_WIDTH][MATRIX_WIDTH] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
};

class Matrix {
public:
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function performs the following matrix multiplication:
	//          matrix1 * matrix2 = productMatrix
	// Parameters: matrix1 - the first matrix
	//             matrix2 - the second matrix
	//             product - the location where the result is stored
	// Returns: nothing
	static void matrixMultiplyMatrix(
			const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH],
			const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH],
			FLOAT product[MATRIX_WIDTH][MATRIX_WIDTH]);

	static void matrixMultiplyMatrixUnoptimized(
			const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH],
			const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH],
			FLOAT product[MATRIX_WIDTH][MATRIX_WIDTH]);

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function multiplies the matrix by the point, storing
	//          the product in result.
	// Parameters: matrix - matrix
	//             point - the 3D point
	//             result - the 3d result
	// Returns: nothing
	static void matrixMultiplyPoint(
			const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH],
			const COORDINATE4 &point, COORDINATE4 &result);

	static void matrixMultiplyPointUnoptimized(
			const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH],
			const COORDINATE4 &point, COORDINATE4 &result);

	static void copy(FLOAT dest[MATRIX_WIDTH][MATRIX_WIDTH], const FLOAT source[MATRIX_WIDTH][MATRIX_WIDTH]);
	static void print(const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH]);
};

#endif /* MATRIX_H_ */
