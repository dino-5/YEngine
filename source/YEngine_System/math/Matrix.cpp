#include "Matrix.h"

Matrix4f increaseDimension(Matrix3f matrix)
{
	Matrix4f r_matrix;
	for (uint i = 0; i < 3; i++)
	{
		for (uint j = 0; j < 3; j++)
			r_matrix[i][j] = matrix[i][j];
		r_matrix[i][3] = 0;
	}
	r_matrix[3][0] = 0;
	r_matrix[3][1] = 0;
	r_matrix[3][2] = 0;
	r_matrix[3][3] = 1;
	return r_matrix;
}

Matrix4f rotationX(float angle)
{
	Matrix4f result;
	result[1][1] = cos(angle);
	result[2][1] = sin(angle);
	result[1][2] = -result[2][1];
	result[2][2] = result[1][1];
	return result;
}

Matrix4f rotationY(float angle)
{
	Matrix4f result;
	result[0][0] = cos(angle);
	result[2][0] = sin(angle);
	result[0][2] = -result[2][0];
	result[2][2] = result[0][0];
	return result;
}

Matrix4f rotationZ(float angle)
{
	Matrix4f result;
	result[0][0] = cos(angle);
	result[1][0] = sin(angle);
	result[0][1] = -result[1][0];
	result[1][1] = result[0][0];
	return result;
}

Matrix4f rotateX(Matrix4f& matrix, float angle)
{
	return matrix * rotationX(angle);
}

Matrix4f rotateY(Matrix4f& matrix, float angle)
{
	return matrix * rotationY(angle);
}

Matrix4f rotateZ(Matrix4f& matrix, float angle)
{
	return matrix * rotationZ(angle);
}

