
#include "testinit.h"

const char* testMatrixMultiplication() {
	static const char* testName = "testMatrixMultlipication";
	const char* failMessage = "Testing of matrix multiplication failed!";
	static const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH] = {
			{1, 2, 3, 4},
			{5, 6, 7, 8},
			{9, 1, 2, 3},
			{4, 5, 6, 7}
	};
	static const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH] = {
			{9, 8, 7, 6},
			{5, 4, 3, 2},
			{1, 2, 3, 4},
			{5, 6, 7, 8}
	};
	FLOAT product1[MATRIX_WIDTH][MATRIX_WIDTH];
	FLOAT product2[MATRIX_WIDTH][MATRIX_WIDTH];

	printf("Testing matrix multiplication...");
	if (TEST_VERBOSE) {
		printf("\nMatrix 1:\n");
		Matrix::print(matrix1);
		printf("Matrix 2:\n");
		Matrix::print(matrix2);
	}
	Matrix::matrixMultiplyMatrixUnoptimized(matrix1, matrix2, product1);
	Matrix::matrixMultiplyMatrix(matrix1, matrix2, product2);

	if (TEST_VERBOSE) {
		printf("Product 1 (unoptimized method):\n");
		Matrix::print(product1);
		printf("Product 2 (optimized method):\n");
		Matrix::print(product2);
	}

	for (unsigned int y = 0; y < MATRIX_WIDTH; ++y)
		for (unsigned int x = 0; x < MATRIX_WIDTH; ++x)
			if (product1[y][x] != product2[y][x]) {
				printf(failMessage);
				printf("\tReason: Found a difference between the unoptimized and optimized multiplication products.\n");
				return testName;
			}
	printf("  Passed!\n");

	return NULL;
}
