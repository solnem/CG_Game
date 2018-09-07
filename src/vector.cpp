#include "vector.h"
#include <assert.h>
#include <math.h>

//#define DEBUG

#define EPSILON 1e-6

#pragma region Constructors

Vector::Vector(float x, float y, float z)
{
	this->X = x;
	this->Y = y;
	this->Z = z;
}

Vector::Vector() {}

#pragma endregion

#pragma region Operators

Vector Vector::operator+(const Vector& v) const
{
	return Vector(this->X + v.X, this->Y + v.Y, this->Z + v.Z);
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(this->X - v.X, this->Y - v.Y, this->Z - v.Z);
}

Vector Vector::operator*(int c) const
{
	return Vector(c * this->X, c * this->Y, c * this->Z);
}

Vector Vector::operator*(float c) const
{
	return Vector(c * this->X, c * this->Y, c * this->Z);
}

Vector Vector::operator/(int c) const
{
	return Vector(this->X / c, this->Y / c, this->Z / c);
}

//Vector Vector::operator*(const Vector& v) const
//{
//	return this->cross(v);
//}

Vector Vector::operator-() const
{
	return Vector(-this->X, -this->Y, -this->Z);
}

Vector& Vector::operator+=(const Vector& v)
{
	this->X = this->X + v.X;
	this->Y = this->Y + v.Y;
	this->Z = this->Z + v.Z;

	return *this;
}

Vector Vector::operator=(const Vector& v)
{
	this->X = v.X;
	this->Y = v.Y;
	this->Z = v.Z;

	return *this;
}

bool Vector::operator<(const Vector& v)
{
	return (this->X < v.X) && (this->Y < v.Y) && (this->Z < v.Z);
}

bool Vector::operator<=(const Vector& v)
{
	return (this->X <= v.X) && (this->Y <= v.Y) && (this->Z <= v.Z);
}

bool Vector::operator>(const Vector& v)
{
	return (this->X > v.X) && (this->Y > v.Y) && (this->Z > v.Z);
}

bool Vector::operator>=(const Vector& v)
{
	return (this->X >= v.X) && (this->Y >= v.Y) && (this->Z >= v.Z);
}

#pragma endregion

#pragma region Methods

float Vector::dot(const Vector& v) const
{
	return this->X * v.X + this->Y * v.Y + this->Z * v.Z;
}

Vector Vector::cross(const Vector& v) const
{
	float newX = this->Y * v.Z - v.Y * this->Z;
	float newY = this->Z * v.X - v.Z * this->X;
	float newZ = this->X * v.Y - v.X * this->Y;

	return Vector(newX, newY, newZ);
}

bool Vector::isNormalized() const 
{
	return (1.f - EPSILON <= this->length() && this->length() <= 1.f + EPSILON) ? true : false;
}

Vector& Vector::normalize()
{
	return this->normalize(false);
}

Vector& Vector::normalize(const bool& inverseSquare)
{
	float length, invSqrtLength;

	if (inverseSquare)
	{
		invSqrtLength = fastInvSqrt(this->lengthSquared());
		//if (invSqrtLength == 0.f) throw std::length_error("Exception: Vector length is 0!\n");
	}
	else
	{
		length = this->length();
		//if (length == 0.f) throw std::length_error("Exception: Vector length is 0!\n");
	}


	if (!this->isNormalized())
	{
		if (inverseSquare)
		{
			this->X *= invSqrtLength;
			this->Y *= invSqrtLength;
			this->Z *= invSqrtLength;
		}
		else
		{
			this->X = this->X / length;
			this->Y = this->Y / length;
			this->Z = this->Z / length;
		}
	}

	return *this;
}

float Vector::length() const
{
	// Using lengthSquared here ensures, that the length is never negative

	return sqrt(lengthSquared());
}

float Vector::lengthSquared() const
{
	return pow(this->X, 2) + pow(this->Y, 2) + pow(this->Z, 2);
}

// Quake 3 Fast Inverse square root (https://en.wikipedia.org/wiki/Fast_inverse_square_root)
float Vector::fastInvSqrt(float squareMagnitude) const
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = squareMagnitude * 0.5f;
	y = squareMagnitude;
	i = *(long *)&y;						// evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);				// what the fuck? 
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));	// 1st iteration
	//y = y * (threehalfs - (x2 * y * y));	// 2nd iteration, this can be removed

	return y;
}

Vector Vector::reflection(const Vector& normal) const
{
	// Project *this onto the normal of the plane
	// Multiply by the negative normal to have it bounce off the plane
	// Double the vector to compensate for the negation
	// Subtract it from this to get the reflected vector
	return *this - normal * 2 * this->dot(normal);
}

bool Vector::triangleIntersection(const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const
{
	Vector normal, point;

	// Calculate the plane of a, b and c
	normal = (b - a).cross(c - a);
	// Normalize it
	normal.normalize(false);

	// Project a onto the normal of the plane
	// Calculate the scalar of *this dependent on the direction vector to the plane
	s = (normal.dot(a) - normal.dot(*this)) / normal.dot(d);

	// Calculate the vector going from the origin (*this) to the point on the plane
	point = *this + d * s;

	if (s > 0.f)
	{
		float abc = fabs((b - a).cross(c - a).length()) / 2.f;
		float abp = fabs((b - a).cross(point - a).length() / 2.f);
		float acp = fabs((c - a).cross(point - a).length() / 2.f);
		float bcp = fabs((c - b).cross(point - b).length() / 2.f);

		return abc + EPSILON >= (abp + acp + bcp);
	}

	return false;
}

#pragma endregion
