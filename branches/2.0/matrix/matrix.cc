
#include "matrix.h"

void Matrix::matrixMultiplyMatrix(
		const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH],
		const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH],
		FLOAT product[MATRIX_WIDTH][MATRIX_WIDTH]) {
	const FLOAT* matrix1Ptr = NULL;
	const FLOAT* matrix1PtrStart = &(matrix1[0][0]);
	const FLOAT* matrix2Ptr = NULL;
	const FLOAT* matrix2PtrStart = NULL;
	const FLOAT* matrix2Saved = &(matrix2[0][0]);
	const FLOAT* matrix2PtrEnd = matrix2Saved + MATRIX_SIZE;
	FLOAT* productPtr = &(product[0][0]);
	FLOAT* productPtrEnd = productPtr + MATRIX_SIZE;
	FLOAT* productPtrEndRow;
	do {
		matrix2PtrStart = matrix2Saved;
		productPtrEndRow = productPtr + MATRIX_WIDTH;
		do {
			matrix1Ptr = matrix1PtrStart;
			matrix2Ptr = matrix2PtrStart;
			*productPtr = 0;
			do {
				*productPtr += *(matrix1Ptr++) * *matrix2Ptr;
				matrix2Ptr += MATRIX_WIDTH;
			} while (matrix2Ptr < matrix2PtrEnd);
			++productPtr;
			++matrix2PtrStart;
		} while (productPtr < productPtrEndRow);
		matrix1PtrStart += MATRIX_WIDTH;
	} while (productPtr < productPtrEnd);
}

void Matrix::matrixMultiplyMatrixUnoptimized(
		const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH],
		const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH],
		FLOAT product[MATRIX_WIDTH][MATRIX_WIDTH]) {
	unsigned int row, column;
	unsigned int index;
	FLOAT cell;

	for (column = 0; column < MATRIX_WIDTH; column++)
	{
		for (row = 0; row < MATRIX_WIDTH; row++)
		{
			cell = 0;
			for (index = 0; index < MATRIX_WIDTH; index++)
			{
				cell += matrix1[column][index] * matrix2[index][row];
			}
			product[column][row] = cell;
		}
	}
}

void Matrix::matrixMultiplyPoint(
		const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH],
		const COORDINATE4 &point, COORDINATE4 &result) {
	const FLOAT* matrixPtr = &(matrix[0][0]);
	const FLOAT* matrixPtrEnd = matrixPtr + MATRIX_SIZE;
	const FLOAT* pointPtr = NULL;
	const FLOAT* pointPtrEnd = point + MATRIX_WIDTH;
	FLOAT* resultPtr = result;

	do {
		pointPtr = point;
		*resultPtr = 0;
		do {
			*resultPtr += *(matrixPtr++) * *(pointPtr++);
		} while (pointPtr < pointPtrEnd);
		++resultPtr;
	} while (matrixPtr < matrixPtrEnd);
}

void Matrix::matrixMultiplyPointUnoptimized(
		const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH],
		const COORDINATE4 &point, COORDINATE4 &result) {
	unsigned int column;
	unsigned int index;

	for (column = 0; column < MATRIX_WIDTH; column++)
	{
		result[column] = 0;
		for (index = 0; index < MATRIX_WIDTH; index++)
		{
			result[column] += matrix[column][index] * point[index];
		}
	}
}

void Matrix::copy(FLOAT dest[MATRIX_WIDTH][MATRIX_WIDTH], const FLOAT source[MATRIX_WIDTH][MATRIX_WIDTH]) {
	memcpy(dest,source,SIZEOF_MATRIX);
}

void Matrix::print(const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH]) {
	unsigned int x, y;

	for (y = 0; y < MATRIX_WIDTH; y++)
	{
		for (x = 0; x < MATRIX_WIDTH; x++)
			printf("%0.6lf ", matrix[y][x]);
		printf("\n");
	}
}
