


#include "volpak_section.h"


#define DEBUG
#undef DEBUG


#ifdef DEBUG
#include <Rcpp.h>
#endif





/* Hyperboloid Methods */


/* C++ note: constructor for pure virtual base class can still be invoked when constructing the derived class */

HyperboloidSection::HyperboloidSection(const Point &  first, const Point &  second, const Point &  third, double & p, double & q) : Section(first, second, third, p, q){

	if (q == 0.0){
		throw std::domain_error("HyperboloidSection not defined for q == 0.0");
	}

}


/* Print method, for debugging */

std::string HyperboloidSection::print() const {

	std::ostringstream msg;

	msg << "HyperboloidSection: " << std::endl;
	msg << Section::print();

	return msg.str();

}



/* length above is a parameter in most of the equations - previously just referred to by a variable 'x'. */

double HyperboloidSection::length_above(double radius) const {

	double x = p * radius / (q - radius);

	if (x < 0.0) {
		x = 0.0;
	}

	return x;

}




double HyperboloidSection::height(double radius) const {

	return first.hag + length_above(first.radius) - length_above(radius);

}




double HyperboloidSection::radius(double ht) const {

	std::ostringstream msg;

	double x = calcx(ht);
	double rad = q * x / (x + p);

	if (rad < 0.0){
		msg << "HyperboloidSection::radius: radius < 0.0 for ht: " << ht << "  x: " << x << std::endl;
		msg << print();
		throw std::domain_error(msg.str());
	}

	return(rad);
}




/* Formula obtained by volume of revolution using the shell method, 
although the disk method would have been just as valid.
*/
double HyperboloidSection::volume(double R, double r) const {

    std::ostringstream msg;
    
	if (R < r){
		std::swap(R, r);
	}


	double vol = 0.0;
	double s = q - r;
	double t = q - R;

	if ((s/t) > 0.0){
		vol = pow(R, 3.0) / t - pow(r, 3.0) / s + 
			(R - r) * (R + r + 2.0 * q) -
			2.0 * q * q * log(s/t);

		vol = M_PI * p * vol;
	}
	else {
		vol = -HUGE_VAL;
	}


	if (vol < 0.0){
	    msg << "HyperboloidSection::conic_volume: vol < 0.0  vol: " << vol << std::endl;
	    msg << this->print();
		throw std::domain_error(msg.str());
	}
		

	return vol;
}



