#include <iostream>
#include <cmath>
#include <string.h>

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
    bool isDotinInterval(double t);
}

class FuzTriangle: public FuzzyFunction
{
    private:
    double middle;

    public:

    void setMiddle(double left, double right)
    {
        middle = left;
    }

    double GetValue(double t)
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
}
}