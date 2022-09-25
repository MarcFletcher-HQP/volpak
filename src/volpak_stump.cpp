

#include "volpak_stump.h"



Stump::Stump(){

    this->ground = Point();
    this->stump = Point();

}



Stump::Stump(const Point & ground, const Point & stump){

    this->ground = ground;
    this->stump = stump;

}



double Stump::stump_vol(){

    return this->volume(ground.radius, stump.radius);

}



Point Stump::midpoint(const Point &  base, const Point &  top){

	double midht = average(base.hag, top.hag);
	double midR = radius(midht);

	return Point(midht, midR);

}