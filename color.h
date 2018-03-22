#ifndef COLOR_H
#define COLOR_H

#include <sstream>

class Color
{
public:
	double r, g, b;

	Color(double R = 0, double G = 0, double B = 0) : r(R), g(G), b(B) {}
	~Color() {}

	void Confine();
	void Init(std::stringstream&);

	friend Color operator + (const Color&, const Color&);
	friend Color operator - (const Color&, const Color&);
	friend Color operator * (const Color&, const Color&);
	friend Color operator * (const Color&, const double&);
	friend Color operator / (const Color&, const double&);
	friend Color& operator += (Color&, const Color&);
	friend Color& operator -= (Color&, const Color&);
	friend Color& operator *= (Color&, const double&);
	friend Color& operator /= (Color&, const double&);
};

#endif
