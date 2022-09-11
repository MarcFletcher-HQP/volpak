

#include "volpak_point.h"



Point::Point(){

    hag = -HUGE_VAL;
    radius = -HUGE_VAL;

}




Point::Point(double ht, double rad){
		
    hag = ht;
    radius = rad;
    
}



/* Copy constructor */
Point::Point(const Point &rhs){
	
	this->hag = rhs.hag;
	this->radius = rhs.radius;

}



/* Assignment constructor */
Point& Point::operator=(const Point &rhs){

	this->hag = rhs.hag;
	this->radius = rhs.radius;

	return *this;

}



bool Point::is_valid(){
    return hag >= 0.0 && radius >= 0.0;
}



bool Point::below(const Point & point){

	return (radius >= point.radius) && (hag < point.hag);

}



bool Point::above(const Point & point){

	return (radius <= point.radius) && (hag > point.hag);

}



std::string print(){

	std::ostringstream msg;

	msg << "(" << hag << ", " << radius << ")";

	return msg.str();

}