


#include "volpak_section.h"


#define DEBUG
#undef DEBUG


#ifdef DEBUG
#include <Rcpp.h>
#endif





/* Cone Methods */


/* ConeSection::ConeSection() : Section(){

} */


ConeSection::ConeSection(const Point &  first, const Point &  second, const Point &  third, double & p, double & q) : Section(first, second, third, p, q){

	double taper = (third.hag - first.hag) / (first.radius - third.radius);

	if (taper == 0.0){ 	// shouldn't ever be true, should this be an error?
		throw std::domain_error("ConeSection has zero taper (Cylinder)");
	}

}



std::string ConeSection::print(){

	std::ostringstream msg;

	msg << "ConeSection: " << std::endl;
	msg << Section::print();

	return msg.str();

}




/* Cone section was not in original volpak code (calcx) - should it really be here now? */

double ConeSection::length_above(double radius){

	double x = radius * (third.hag - first.hag) / (first.radius - third.radius);

	if (x < 0.0) {
		x = 0.0;
	}

	return x;

}




double ConeSection::height(double radius){

	double ratio = (radius - third.radius) * (third.hag - first.hag) / (first.radius - third.radius);
	return third.hag - ratio;

}




double ConeSection::radius(double ht){

    std::ostringstream msg;
	
	double ratio =  (ht - first.hag) * (first.radius - third.radius) / (third.hag - first.hag);
	double rad = first.radius - ratio;

	if (rad < 0.0){
		msg << "ConeSection::radius: radius < 0.0 for ht: " << ht << std::endl;
		msg << this->print();
		throw std::domain_error(msg.str());
	}

	return(rad);
}




double ConeSection::volume(double R, double r){
	
	std::ostringstream msg;


	if (R < r){
		std::swap(R, r);
	}


	double taper = (third.hag - first.hag) / (first.radius - third.radius);
	double vol = (M_PI / 3.0) * abs(taper) * (pow(R, 3.0) - pow(r, 3.0));


	if (vol < 0.0){
	    msg << "ConeSection::conic_volume: vol < 0.0  vol: " << vol << std::endl;
	    msg << this->print();
		throw std::domain_error(msg.str());
	}

	return vol;
}



