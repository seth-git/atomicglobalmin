
#include "matrix.h"

void Matrix::matrixMultiplyMatrix(const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH],
                                  const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH],
                                  FLOAT product[MATRIX_WIDTH][MATRIX_WIDTH]) {
	unsigned int width, height;
	unsigned int index;
	FLOAT cell;

	for (width = 0; width < MATRIX_WIDTH; width++)
	{
		for (height = 0; height < MATRIX_WIDTH; height++)
		{
			cell = 0;
			for (index = 0; index < MATRIX_WIDTH; index++)
			{
				cell += matrix1[index][height] * matrix2[width][index];
			}
			product[width][height] = cell;
		}
	}
}

void Matrix::matrixMultiplyPoint(COORDINATE4 &point, const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH]) {
	unsigned int height;
	unsigned int index;
	COORDINATE4 originalPoint;
	FLOAT* cell = point;

	memcpy(originalPoint,point,sizeof(originalPoint));
	for (height = 0; height < MATRIX_WIDTH; height++)
	{
		*cell = 0;
		for (index = 0; index < MATRIX_WIDTH; index++)
		{
			*cell += matrix[index][height] * originalPoint[index];
		}
		++cell;
	}
}

void Matrix::copy(FLOAT dest[MATRIX_WIDTH][MATRIX_WIDTH], const FLOAT source[MATRIX_WIDTH][MATRIX_WIDTH]) {
	memcpy(dest,source,sizeof(dest));
}

void Matrix::printMatrix(const FLOAT matrix[MATRIX_WIDTH][MATRIX_WIDTH]) {
	unsigned int x, y;

	for (y = 0; y < MATRIX_WIDTH; y++)
	{
		for (x = 0; x < MATRIX_WIDTH; x++)
			printf("%0.6lf ", matrix[x][y]);
		printf("\n");
	}
}
