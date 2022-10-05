

#include "volpak_section.h"


#define DEBUG
#undef DEBUG


#ifdef DEBUG
#include <Rcpp.h>
#endif





/* Paraboloid Methods */


/* C++ note: constructor for pure virtual base class can still be invoked when constructing the derived class */

ParaboloidSection::ParaboloidSection(const Point &  first, const Point &  second, const Point &  third, double & p, double & q) : Section(first, second, third, p, q){

	std::ostringstream msg;

	if (q == 0.0){
		throw std::domain_error("ParaboloidSection not defined for q == 0.0");
	}

}



/* Print method, for debugging */

std::string ParaboloidSection::print() const {

	std::ostringstream msg;

	msg << "ParaboloidSection: " << std::endl;
	msg << Section::print();

	return msg.str();

}



/* length above is a parameter in most of the equations - previously just referred to by a variable 'x'. */

double ParaboloidSection::length_above(double radius) const {

	double x = (p - radius) * radius / q;

	if (x < 0.0) {
		x = 0.0;
	}

	return x;

}




double ParaboloidSection::height(double radius) const {

	return first.hag + length_above(first.radius) - length_above(radius);

}




double ParaboloidSection::radius(double ht) const {

	std::ostringstream msg;

	double z = discriminant(ht);

	if(z < 0){
		msg << "ParaboloidSection::radius: Discriminant < 0.0 for ht: " << ht << std::endl;
		msg << print() << std::endl;
		throw std::domain_error(msg.str());
	}


	double rad = (p - sqrt(z)) / 2.0;

	if (rad < 0.0){
		msg << "ParaboloidSection::radius: radius < 0.0 for ht: " << ht << "  rad: " << rad << std::endl;
		msg << print();
		throw std::domain_error(msg.str());
	}

	return(rad);
}




/* Formula obtained by volume of revolution using the shell method,
disk method not feasible in this case as the function expressing
radius in terms of height is not invertible.
*/
double ParaboloidSection::volume(double R, double r) const {

    std::ostringstream msg;

	if (R < r){
		std::swap(R, r);
	}


	double vol = p * (pow(R, 3.0) - pow(r, 3.0)) / 3.0 -
		(pow(R, 4.0) - pow(r, 4.0)) / 2.0;

	vol = M_PI * vol / q;


	if (vol < 0.0){
	    msg << "ParaboloidSection::conic_volume: vol < 0.0  vol: " << vol << std::endl;
	    msg << print();
		throw std::domain_error(msg.str());
	}


	return vol;
}



