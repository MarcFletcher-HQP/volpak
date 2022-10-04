

#include "volpak_stump.h"


/* Default constructor only required because NeiloidStump needs 'ground' and 'stump' to
be initialised...for some reason. */

Stump::Stump(){

    this->ground = Point();
    this->stump = Point();

}



Stump::Stump(const Point & ground, const Point & stump){

    this->ground = ground;
    this->stump = stump;

}



double Stump::stump_vol() const {

    return volume(ground.radius, stump.radius);

}

