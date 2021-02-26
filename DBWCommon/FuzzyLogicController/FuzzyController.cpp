#include "FuzzyController.h"

namespace FuzzyController
{
FuzzyFunction::FuzzyFunction()
{
	//constructor
	left_point = 0;
	right_point = 0;
}
void FuzzyFunction::setInterval(double left, double right)
{
	left_point = left;
	right_point = right;
}
void FuzzyFunction::setMiddle(double left,double right)
{
	//do nothing
}
double FuzzyFunction::GetValue(double t)
{
	//do nothing
}
void FuzzyFunction::setName(std::string s)
{
	name = s;
}
bool FuzzyFunction::isDotinInterval(double t)
{
	if(left_point < t && t < right_point)
		return true;
	else
		return false;
}
void FuzzyFunction::GetName(std::string &s)
{
	s = name;
}
FuzzyFunction::~FuzzyFunction()
{
	//Destructor
}

void FuzTriangle::setMiddle(double left,double right)
{
	middle = left;
}

double FuzTriangle::GetValue(double t)
{
	if(t <= left_point)
	{
		return 0;
	}
	else if(t < middle)
	{
		return ((t-left_point)/(middle-left_point));
	}
	else if(t == middle)
	{
		return 1.0;
	}
	else if(t < right_point)
	{
		return ((right_point-t)/(right_point - middle));
	}
	else
	{
		return 0;
	}
}

void FuzTrapezoid::setMiddle(double left, double right)
{
	l_middle = left;
	r_middle = right;
}
double FuzTrapezoid::GetValue(double t)
{
	if(t <= left_point)
	{
		return 0;
	}
	else if(t < l_middle)
	{
		return ((t-left_point)/(l_middle-left_point));
	}
	else if(t <= r_middle)
	{
		return 1.0;
	}
	else if(t < right_point)
	{
		return ((right_point-t)/(right_point - r_middle));
	}
	else
	{
		return 0;
	}
}
}
