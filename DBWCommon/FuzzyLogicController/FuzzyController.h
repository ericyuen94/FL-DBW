#include <iostream>
#include <cmath>
#include <string>

namespace FuzzyController
{
class FuzzyFunction
{
protected:
	double left_point;
	double right_point;
	std::string name;

public:
	FuzzyFunction();
	virtual ~FuzzyFunction();

	virtual void setInterval(double left, double right);
	virtual void setMiddle(double left, double right);
	virtual void setName(std::string s);
	virtual double GetValue(double t);
	virtual bool isDotinInterval(double t);
	void GetName(std::string &s);
};

class FuzTriangle: public FuzzyFunction
{
private:
	double middle;

public:
	void setMiddle(double left, double right);
	double GetValue(double t);

};

class FuzTrapezoid: public FuzzyFunction
{
private:
	double l_middle;
	double r_middle;

public:
	void setMiddle(double left, double right);
	double GetValue(double t);
	bool isDotinInterval(double t);


};

class FuzzySolution: public FuzzyFunction
{
public:
	double out_data;
	void SetValue(double output)
	{
		out_data = output;
	}
};
}
