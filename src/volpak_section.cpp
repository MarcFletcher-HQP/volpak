

#include "volpak_section.h"


#define DEBUG
#undef DEBUG


#ifdef DEBUG
#include <Rcpp.h>
#endif




/* Section Methods - Shared by all derived classes */



Section::Section(){

	this->first = Point();
	this->second = Point();
	this->third = Point();
	this->p = -HUGE_VAL;
	this->q = -HUGE_VAL;

}


Section::Section(const Point &  first, const Point &  second, const Point &  third, double & p, double & q){

	this->first = first;
	this->second = second;
	this->third = third;
	this->p = p;
	this->q = q;

}





double Section::calcx(double ht) const {

	double length = first.hag + length_above(first.radius);
	return length - ht;

}




double Section::total_volume() const {

	double vol = (M_PI / 6.0) *
		(pow(first.radius, 2.0) + 4.0 * pow(second.radius, 2.0) + pow(third.radius, 2.0)) *
		(third.hag - first.hag);

	return vol;

}




bool Section::contains_radius(double rad) const {

	return ((rad >= third.radius) && (rad <= first.radius)) || (abs(first.radius - rad) < RADTOL) || (abs(third.radius - rad) < RADTOL);

}




bool Section::contains_height(double ht) const {

	return ((ht <= third.hag) && (ht >= first.hag)) || (abs(third.hag - ht) < HTTOL) || (abs(first.hag - ht) < HTTOL);

}


/* Print method, for debugging */

std::string Section::print() const {

	std::ostringstream msg;

	msg << "\t" << "first:  " << first.print() << std::endl;
	msg << "\t" << "second: " << second.print() << std::endl;
	msg << "\t" << "third:  " << third.print() << std::endl;
	msg << "\t" << "p: " << p << std::endl;
	msg << "\t" << "q: " << q << std::endl;

	return msg.str();

}




Point Section::midpoint() const {

	double midht = average(first.hag, third.hag);
	double midR = radius(midht);

	return Point(midht, midR);

}




double Section::discriminant(double ht) const {

	double x = calcx(ht);

	return p * p - 4.0 * q * x;

}
