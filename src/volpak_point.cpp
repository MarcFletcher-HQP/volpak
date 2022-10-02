

#include "volpak_point.h"



Point::Point(){

    hag = -HUGE_VAL;
    radius = -HUGE_VAL;

}




Point::Point(double ht, double rad){

    hag = ht;
    radius = rad;

}




bool Point::is_valid() const {
    return hag >= 0.0 && radius >= 0.0;
}



bool Point::below(const Point & point) const {

	return (radius >= point.radius) && (hag < point.hag);

}



bool Point::above(const Point & point) const {

	return (radius <= point.radius) && (hag > point.hag);

}



std::string Point::print() const {

	std::ostringstream msg;

	msg << "(" << hag << ", " << radius << ")";

	return msg.str();

}
