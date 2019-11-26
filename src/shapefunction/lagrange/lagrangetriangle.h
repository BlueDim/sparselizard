// sparselizard - Copyright (C) 2017- A. Halbach, University of Liege
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to <alexandre.halbach at gmail.com>.


#ifndef LAGRANGETRIANGLE_H
#define LAGRANGETRIANGLE_H

#include <vector>
#include "polynomial.h"
#include "math.h"
#include "element.h"


namespace lagrangetriangle
{
    // 'getnodecoordinates' gives the ki, eta and phi 
    // coordinates of every Lagrange node for the required order:
    std::vector<double> getnodecoordinates(int order);
    // 'getformfunctionpolynomials' outputs a vector giving the form function polynomials:
    std::vector<polynomial> getformfunctionpolynomials(int order);
}

#endif
