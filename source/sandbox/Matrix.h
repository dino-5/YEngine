#pragma once
#include <array>
#include <iostream>
#include "Vector.h" 

#define MatrixTemplate template<typename T, int dimNumber>
#define MatrixImp Matrix<T, dimNumber>
#define iLoop() for(int i=0; i<dimNumber; i++)
#define jLoop() for(int j=0; j<dimNumber; j++)

template<typename T>
void swap(T & first, T & second)
{
	T tmp;
	tmp = first;
	first = second;
	second = tmp;
}

template<typename T, int dimNumber>
class Matrix
{
public:
	Matrix()
	{
		iLoop()
		{
			m_matrix[i][i] = 1;
		}
	}
	Vector<T, dimNumber>& operator[](int index) { return m_matrix[index]; }
	void print();
	Matrix transpose();
	void selfTranspose();
	Matrix operator*(Matrix&& m2)
	{
		Matrix result;
		iLoop()
		{
			jLoop()
			{
				result[i][j] += m_matrix[i][j] * m2[j][i];
			}
		}
		return result;
	}
public:
	std::array<Vector<T, dimNumber>, dimNumber> m_matrix{};
};

MatrixTemplate
MatrixImp operator*(MatrixImp& m1, MatrixImp& m2)
{
	MatrixImp result;
	iLoop()
	{
		jLoop()
		{
			result[i][j] += m1[i][j] * m2[j][i];
		}
	}
	return result;
}


#define MatrixType(name, type, number) using Matrix##name = Matrix<type, number>;
MatrixType(4f, float, 4)
MatrixType(3f, float, 3)
MatrixType(2f, float, 2)

Matrix4f increaseDimension(Matrix3f matrix);
Matrix4f rotationX(float angle);
Matrix4f rotationY(float angle);
Matrix4f rotationZ(float angle);
Matrix4f rotateX(const Matrix4f& matrix, float angle);
Matrix4f rotateY(const Matrix4f& matrix, float angle);
Matrix4f rotateZ(const Matrix4f& matrix, float angle);

MatrixTemplate
MatrixImp MatrixImp::transpose()
{
	MatrixImp result;
	iLoop()
	{
		jLoop()
		{
			result[i][j] = m_matrix[j][i];
		}
	}
	return result;
}

MatrixTemplate
void MatrixImp::selfTranspose()
{
	MatrixImp result;
	iLoop()
	{
		for(int j=0; j<i;j++)
		{
			swap(m_matrix[i][j], m_matrix[j][i]);
		}
	}
}

MatrixTemplate
void MatrixImp::print()
{
	std::cout << std::endl;
	iLoop()
	{
		jLoop()
		{
			std::cout << m_matrix[i][j] << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
