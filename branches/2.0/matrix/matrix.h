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

class Matrix {
public:
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function performs the following matrix multiplication:
	//          matrix1 * matrix2 = productMatrix
	// Parameters: matrix1 - the first matrix
	//             matrix2 - the second matrix
	//             product - the location where the result is stored
	// Returns: nothing
	static void matrixMultiplyMatrix(const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH],
	                                 const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH],
	                                 FLOAT product[MATRIX_WIDTH][MATRIX_WIDTH]);

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function multiplies the 3D "point" by the matrix and
	//          stores the result in point.
	// Parameters: point - the 3D point
	//             matrix - matrix
	// Returns: nothing
	static void matrixMultiplyPoint(COORDINATE4 &point, const double matrix[MATRIX_WIDTH][MATRIX_WIDTH]);

	void copy(FLOAT dest[MATRIX_WIDTH][MATRIX_WIDTH], const FLOAT source[MATRIX_WIDTH][MATRIX_WIDTH]);
	void printMatrix(const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH]);
};

#endif /* MATRIX_H_ */
