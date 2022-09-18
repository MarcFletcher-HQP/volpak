

#ifndef VOLPAK_H
#define VOLPAK_H


#include <cmath>
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>


//[[Rcpp::plugins("cpp11")]]



/* Defines */

#define NEWTON
#define STUMPHT 0.15



/* Tolerances */

static double HTTOL = 0.1;
static double RADTOL = 0.05/200;



/* Various functions */

double average(double low, double high);



#endif