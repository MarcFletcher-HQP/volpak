


#include "volpak_stump.h"





/* ParaboloidStump Methods */


ParaboloidStump::ParaboloidStump(const Point& ground, const Point& stump, const Section& base){

    this->ground = ground;
    this->stump = stump;
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




double ParaboloidStump::volume(Point point){

    return base.volume(rad);

}




double ParaboloidStump::vol_stump_to_first_meas(){

    return base.volume(base.first) - base.volume(stump);

}



