


#include "volpak_stump.h"





/* ParaboloidStump Methods */


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

    return base.contains_radius(rad);

}




bool ParaboloidStump::contains_height(double ht) const {

    return base.contains_height(ht);

}
