#include "FuzzyController.h"

namespace FuzzyController
{
    FuzzyController::FuzzyController()
    {
        //constructor
    }
    void FuzzyController::setInterval(double left, double right)
    {
        left_point = left;
        right_point = right;
    }
    void FuzzyController::setName(string s)
    {
        name = s;
    }
    bool FuzzyController::isDotinInterval(double t)
    {
        if(left_point < t && t < right_point)
            return true;
        else
            return false;
    }
    FuzzyController::~FuzzyController()
    {
        //Destructor
    }
}