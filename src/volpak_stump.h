/*  */


#include <math.h>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>


#include "volpak.h"
#include "volpak_point.h"
#include "volpak_section.h"




#ifndef STUMP_H
#define STUMP_H


class Stump {

    public:

        virtual std::string print();
        virtual double radius(double height);
        virtual double height(double radius);
        virtual double volume(Point point);
        virtual double vol_stump_to_first_meas();

        virtual ~Stump(){};

};




class NeiloidStump : public Stump {

    public:

        virtual std::string print();
        virtual double radius(double height);
        virtual double height(double radius);
        virtual double volume(Point point);
        virtual double vol_stump_to_first_meas();

        NeiloidStump(const Point& first, const Point& second);
        virtual ~NeiloidStump(){};

    public:

        Point ground;           // as written on the can
        Point stump;            // top of the stump
        Point first;            // lowest stem measure on the tree
        Point second;           // second stem measure

};



class ParaboloidStump : public Stump {

    public:

        virtual std::string print();
        virtual double radius(double height);
        virtual double height(double radius);
        virtual double volume(Point point);
        virtual double vol_stump_to_first_meas();

        ParaboloidStump(const Point& ground, const Point& stump, const Section& base);
        virtual ~ParaboloidStump(){};

    public:

        Point ground;              // as written on the can
        Point stump;               // top of the stump
        ParaboloidSection base;    // ParaboloidSection defined by the first three measures

};




#endif