


#include "volpak_stump.h"



/* ParaboloidStump Methods - Mostly just wrapper functions around the ParaboloidSection, stored for the Stump.*/


/* C++ note: constructor for pure virtual base class can still be invoked when constructing the derived class */

ParaboloidStump::ParaboloidStump(const Point& ground, const Point& stump, ParaboloidSection & base) : Stump(ground, stump){

    this->base = base;

}



std::string ParaboloidStump::print() const {

    std::ostringstream msg;

    msg << "ParaboloidStump: " << std::endl;
    msg << "\t" << "ground: " << ground.print() << std::endl;
    msg << "\t" << "stump: " << stump.print() << std::endl;
    msg << "\t" << base.print();

    return msg.str();

}




double ParaboloidStump::radius(double ht) const {

    return base.radius(ht);

}




double ParaboloidStump::height(double rad) const {

    return base.height(rad);

}




double ParaboloidStump::volume(double r1, double r2) const {

    return base.volume(r1, r2);

}




double ParaboloidStump::total_volume() const {

    return base.volume(ground.radius, base.first.radius);

}




bool ParaboloidStump::contains_radius(double rad) const {

	return ((rad >= base.first.radius) && (rad <= ground.radius)) || (abs(ground.radius - rad) < RADTOL) || (abs(base.first.radius - rad) < RADTOL);

}




bool ParaboloidStump::contains_height(double ht) const {

	return ((ht <= base.first.hag) && (ht >= ground.hag)) || (abs(base.first.hag - ht) < HTTOL) || (abs(ground.hag - ht) < HTTOL);

}



