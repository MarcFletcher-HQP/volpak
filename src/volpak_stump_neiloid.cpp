/*  */



#include "volpak_stump.h"



/* NeiloidStump Methods:

Extrapolation of ground radius using newtons' method.

 Assuming a neiloid shape for the base of the tree:
 a * r^2 = ht^3

 Not knowing an appropriate value of 'a', newtons' method is
 used to estimate the rate of change in r^(2/3).
 */


NeiloidStump::NeiloidStump(const Point& first, const Point& second) {

    /* Can't do radius calc without initialising first and second */

    this->first = first;
    this->second = second;


    double rad = this->radius(0.0);

    ground = Point(0.0, rad);


    if (STUMPHT > 0.0){
        rad = this->radius(STUMPHT);
    }
    else {
        rad = ground.radius;
    }


    if (rad < 0.0){
		throw std::domain_error("Tree::set_neiloid_stump: Neiloid model produced a stump radius < 0.0");
    }

    stump = Point(STUMPHT, rad);

}



std::string NeiloidStump::print() const {

    std::ostringstream msg;

    msg << "NeiloidStump: " << std::endl;
    msg << "\t" << "ground: " << ground.print() << std::endl;
    msg << "\t" << "stump: " << stump.print() << std::endl;
    msg << "\t" << "first: " << first.print() << std::endl;
    msg << "\t" << "second: " << second.print() << std::endl;

    return msg.str();

}





double NeiloidStump::radius(double ht) const {

    std::ostringstream msg;

    if (ht > second.hag){
        throw std::invalid_argument("NeiloidStump::radius: Neiloid model is not appropriate for points above the second measure\n");
    }

    double twothirds = 2.0 / 3.0;
    double rtrans = pow(first.radius, twothirds);
    double dif = (rtrans - pow(second.radius, twothirds)) / (second.hag - first.hag);

    if (dif < 0.0){
        dif = 0.0;
    }

    double rad = pow(rtrans + dif * (first.hag - ht), 1.5);


    if (rad < 0.0){
		msg << "NeiloidStump::radius: stump radius < 0.0;  rad: " << rad << "  ht: " << ht << std::endl;
        msg << "\tdif = " << dif << "  rtrans = " << rtrans << std::endl;
        msg << print();
		throw std::domain_error(msg.str());
    }

    return rad;

}




double NeiloidStump::height(double rad) const {

	std::ostringstream msg;

    if (rad < second.radius){
        throw std::invalid_argument("NeiloidStump::height: Neiloid model is not appropriate for points above the second measure\n");
    }

    // For a neiloid, the height is proportional to radius^(2/3)
    double twothirds = 2.0 / 3.0;
    double r0Lin = pow(ground.radius, twothirds);
    double radiLin = pow(rad, twothirds);
    double ht = first.hag * (r0Lin - radiLin) / (r0Lin - pow(first.radius, twothirds));


    if (ht < 0.0){
		msg << "NeiloidStump::height: stump radius < 0.0;  rad: " << rad << "  ht: " << ht << std::endl;
        msg << print();
		throw std::domain_error(msg.str());
    }

    return ht;
}





/* Calculate the volume for a point below the first measure */

double NeiloidStump::volume(double r1, double r2) const {              /* Question: does r1 have to be ground.radius? */

	double ht = height(r2);
    double twothirds = 2.0 / 3.0;


    // For a neiloid, the height is proportional to radius^(2/3)
    double r0Lin = pow(r1, twothirds);
    double radiLin = pow(r2 , twothirds);


    /* Equation obtained from the volume of revolution of a neiloid, using the shell method (as per the equivalent calculation
        for a paraboloid and hyperboloid in 'volume' method of relevant sections.
        */
    double vol = M_PI_4 * ht * (r0Lin * r0Lin + radiLin * radiLin) * (r0Lin + radiLin);

    return vol;

}



double NeiloidStump::total_volume() const {

    return volume(ground.radius, first.radius);

}




bool NeiloidStump::contains_radius(double rad) const {

	return ((rad >= first.radius) && (rad <= ground.radius)) || (abs(ground.radius - rad) < RADTOL) || (abs(first.radius - rad) < RADTOL);

}




bool NeiloidStump::contains_height(double ht) const {

	return ((ht <= first.hag) && (ht >= ground.hag)) || (abs(first.hag - ht) < HTTOL) || (abs(ground.hag - ht) < HTTOL);

}



