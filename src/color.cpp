#include "color.h"
#include <assert.h>

//#define DEBUG

#pragma region Constructors

Color::Color()
{
	this->R = 0.f;
	this->G = 0.f;
	this->B = 0.f;
}

Color::Color( float r, float g, float b)
{
	// Making sure values stay in color range
	//this->R = clampColorRange(r);
	//this->G = clampColorRange(g);
	//this->B = clampColorRange(b);

	this->R = r;
	this->G = g;
	this->B = b;
}

#pragma endregion

#pragma region Operators

Color Color::operator*(const Color& c) const
{
	//return Color(
	//	clampColorRange(this->R * c.R),
	//	clampColorRange(this->G * c.G),
	//	clampColorRange(this->B * c.B));

	return Color(this->R * c.R, this->G * c.G, this->B * c.B);
}

Color Color::operator*(const float Factor) const
{
	//return Color(
	//	clampColorRange(this->R * Factor),
	//	clampColorRange(this->G * Factor),
	//	clampColorRange(this->B * Factor));

	return Color(this->R * Factor, this->G * Factor, this->B * Factor);
}

Color Color::operator+(const Color& c) const
{
	//return Color(
	//	clampColorRange(this->R + c.R),
	//	clampColorRange(this->G + c.G),
	//	clampColorRange(this->B + c.B));

	return Color(this->R + c.R, this->G + c.G, this->B + c.B);
}

Color& Color::operator+=(const Color& c)
{
	//this->R = clampColorRange(this->R + c.R);
	//this->G = clampColorRange(this->G + c.G);
	//this->B = clampColorRange(this->B + c.B);

	this->R = this->R + c.R;
	this->G = this->G + c.G;
	this->B = this->B + c.B;

	return *this;
}

Color& Color::operator*=(const Color& c)
{
	//this->R = clampColorRange(this->R * c.R);
	//this->G = clampColorRange(this->G * c.G);
	//this->B = clampColorRange(this->B * c.B);

	this->R = this->R * c.R;
	this->G = this->G * c.G;
	this->B = this->B * c.B;

	return *this;
}

Color& Color::operator*=(const float Factor)
{
	//this->R = clampColorRange(this->R * Factor);
	//this->G = clampColorRange(this->G * Factor);
	//this->B = clampColorRange(this->B * Factor);

	this->R = this->R * Factor;
	this->G = this->G * Factor;
	this->B = this->B * Factor;

	return *this;
}

#pragma endregion

#pragma region Methods

float Color::clampColorRange(const float value)
{
	return (value < 0.f) ? 0.f : (value > 1.f) ? 1.f : value;
}

#pragma endregion
