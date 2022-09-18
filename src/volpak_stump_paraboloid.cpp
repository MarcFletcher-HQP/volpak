


#include "volpak_stump.h"





/* ParaboloidStump Methods */


ParaboloidStump::ParaboloidStump(const Point& ground, const Point& stump, ParaboloidSection & base) : Stump(ground, stump){

    this->base = base;

}



std::string ParaboloidStump::print(){

    std::ostringstream msg;

    msg << "ParaboloidStump: " << std::endl;
    msg << "\t" << "ground: " << ground.print() << std::endl;
    msg << "\t" << "stump: " << stump.print() << std::endl;
    msg << "\t" << base.print();

    return msg.str();

}




double ParaboloidStump::radius(double ht){

    return base.radius(ht);

}




double ParaboloidStump::height(double rad){

    return base.height(rad);

}




double ParaboloidStump::volume(double r1, double r2){

    return base.volume(r1, r2);

}




double ParaboloidStump::total_volume(){

    return base.volume(ground.radius, base.first.radius);

}




bool ParaboloidStump::contains_radius(double rad){

    return base.contains_radius(rad);

}




bool ParaboloidStump::contains_height(double ht){

    return base.contains_height(ht);

}
