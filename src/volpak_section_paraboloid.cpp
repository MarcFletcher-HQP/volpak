

#include "volpak_section.h"


#define DEBUG
#undef DEBUG


#ifdef DEBUG
#include <Rcpp.h>
#endif





/* Paraboloid Methods */


ParaboloidSection::ParaboloidSection(Point first, Point second, Point third, double p, double q) : Section(first, second, third, p, q){

	if (q == 0.0){
		throw std::domain_error("ParaboloidSection not defined for q == 0.0");
	}

	if ( pow(p, 2.0) - 4 * q * calcx(first.radius) <= 0.0 ){
		throw std::domain_error("ParaboloidSection: no real solution for radius along entire section.");
	}

}




double ParaboloidSection::length_above(double radius){

	double x = (p - radius) * radius / q;

	if (x < 0.0) {
		x = 0.0;
	}

	return x;

}




double ParaboloidSection::height(double radius){
	
	return length_above(first.radius) - length_above(radius);

}




double ParaboloidSection::radius(double ht){

	std::ostringstream msg;

	double z = pow(p, 2.0) - 4 * q * calcx(ht);
	double rad = (p - sqrt(z)) / 2.0;

	if (rad < 0.0){
		msg << "ParaboloidSection::radius: radius < 0.0 for ht: " << ht << "  x: " << x << std::endl;
		msg << this->print();
		throw std::domain_error(msg.str());
	}

	return(rad);
}




/* Formula obtained by volume of revolution using the shell method, 
disk method not feasible in this case as the function expressing 
radius in terms of height is not invertible.
*/
double ParaboloidSection::volume(double R, double r){

    std::ostringstream msg;
    
	if (R < r){
		std::swap(R, r);
	}


	double vol = p * (pow(R, 3.0) - pow(r, 3.0)) / 3.0 - 
		(pow(R, 4.0) - pow(r, 4.0)) / 2.0;

	vol = M_PI * vol / q;

	
	if (vol < 0.0){
	    msg << "ParaboloidSection::conic_volume: vol < 0.0  vol: " << vol << std::endl;
	    msg << this->print();
		throw std::domain_error(msg.str());
	}
	
	
	return vol;
}



