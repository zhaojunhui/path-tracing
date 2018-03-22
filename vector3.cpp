#include "vector3.h"
#include <cmath>
#include <cstdlib>

const double EPS = 1e-6;
const double PI = 3.1415926535897932384626;

double Vector3::Rand01d()
{
	return double(rand() % 65535) / 65535;
}

double Vector3::Dot(const Vector3& A) 
{
	return x * A.x + y * A.y + z * A.z;
}

double Vector3::Module2() 
{
	return x * x + y * y + z * z;
}

double Vector3::Module() 
{
	return sqrt(x * x + y * y + z * z);
}

double Vector3::Distance2(Vector3& A) 
{
	return (A - *this).Module2();
}

double Vector3::Distance(Vector3& A) 
{
	return (A - *this).Module();
}

Vector3 Vector3::Normalize() 
{
	return *this / Module();
}

Vector3 Vector3::GetAnVerticalVector() 
{
	Vector3 ans = *this * Vector3(0, 0, 1);
	if(ans.IsZeroVector()) ans = Vector3(1, 0, 0);
		else ans = ans.Normalize();
	return ans;
}

bool Vector3::IsZeroVector() 
{
	return (fabs(x) < EPS) && (fabs(y) < EPS) && (fabs(z) < EPS);
}

void Vector3::Init(std::stringstream& fin) 
{
	fin >> x >> y >> z;
}

Vector3 Vector3::Reflect(Vector3 N) 
{
	return *this - N * (2 * Dot(N));
} 

Vector3 Vector3::Refract(Vector3 N, double n) 
{
	Vector3 V = Normalize();
	double cosI = -N.Dot(V), cosT2 = 1 - (n * n) * (1 - cosI * cosI); 
	if(cosT2 > EPS) return V * n + N * (n * cosI - sqrt(cosT2));
	return V.Reflect(N);
}

Vector3 Vector3::Diffuse() 
{
	Vector3 Vert = GetAnVerticalVector();
	double theta = acos(sqrt(Rand01d()));
	double phi = Rand01d() * 2 * PI;
	return Rotate(Vert, theta).Rotate(*this, phi);
}

Vector3 Vector3::Rotate(Vector3 axis, double theta) 
{
	Vector3 ans;
	double cost = cos(theta);
	double sint = sin(theta);
	ans.x += x * (axis.x * axis.x + (1 - axis.x * axis.x) * cost);
	ans.x += y * (axis.x * axis.y * (1 - cost) - axis.z * sint);
	ans.x += z * (axis.x * axis.z * (1 - cost) + axis.y * sint);
	ans.y += x * (axis.y * axis.x * (1 - cost) + axis.z * sint);
	ans.y += y * (axis.y * axis.y + (1 - axis.y * axis.y) * cost);
	ans.y += z * (axis.y * axis.z * (1 - cost) - axis.x * sint);
	ans.z += x * (axis.z * axis.x * (1 - cost) - axis.y * sint);
	ans.z += y * (axis.z * axis.y * (1 - cost) + axis.x * sint);
	ans.z += z * (axis.z * axis.z + (1 - axis.z * axis.z) * cost);
	return ans;
}

Vector3 operator + (const Vector3& A, const Vector3& B) 
{
	return Vector3(A.x + B.x, A.y + B.y, A.z + B.z);
}

Vector3 operator - (const Vector3& A, const Vector3& B) 
{
	return Vector3(A.x - B.x, A.y - B.y, A.z - B.z);
}

Vector3 operator * (const Vector3& A, const double& k) 
{
	return Vector3(A.x * k, A.y * k, A.z * k);
}

Vector3 operator / (const Vector3& A, const double& k) 
{
	return Vector3(A.x / k, A.y / k, A.z / k);
}

Vector3 operator * (const Vector3& A, const Vector3& B) 
{
	return Vector3(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x);
}

Vector3& operator += (Vector3& A, const Vector3& B) 
{
	A = A + B;
	return A;
}

Vector3& operator -= (Vector3& A, const Vector3& B) 
{
	A = A - B;
	return A;
}

Vector3& operator *= (Vector3& A, const double& k) 
{
	A = A * k;
	return A;
}

Vector3& operator += (Vector3& A, const double& k) 
{
	A = A / k;
	return A;
}

Vector3& operator *= (Vector3& A, const Vector3& B) 
{
	A = A * B;
	return A;
}

Vector3 operator - (const Vector3& A) 
{
	return Vector3(-A.x, -A.y, -A.z);
}
