#pragma once
#include <array>

using uint = unsigned int;

template<typename T, int dimNumber>
class Vector
{
public:
	Vector() = default;
	T& operator[](int index) { return m_vector[index]; }
	void print();

public:
	std::array<T, dimNumber> m_vector{};
};

using Vector4f = Vector<float, 4>;

#include <iostream>

template<typename T, int dimNumber>
void Vector<T, dimNumber>::print()
{
	std::cout << std::endl;
	for (int i = 0; i < dimNumber; i++)
	{
		std::cout << m_vector[i] << ' ';
	}
	std::cout << std::endl;
}

