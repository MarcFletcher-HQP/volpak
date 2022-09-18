/*  */


#include <memory>


#include "volpak.h"
#include "volpak_point.h"
#include "volpak_section.h"




#ifndef STUMP_H
#define STUMP_H


class Stump {

    public:

        double stump_vol();

        Stump();
        Stump(const Point & ground, const Point & stump);

        virtual std::string print() = 0;
        virtual double radius(double height) = 0;
        virtual double height(double radius) = 0;
        virtual double volume(double r1, double r2) = 0;
        virtual double total_volume() = 0;

        virtual bool contains_radius(double rad) = 0;
	    virtual bool contains_height(double ht) = 0;

        virtual ~Stump(){};


    public:

        Point ground;              // as written on the can
        Point stump;               // top of the stump

};




class NeiloidStump : public Stump {

    public:

        virtual std::string print();
        virtual double radius(double height);
        virtual double height(double radius);
        virtual double volume(double r1, double r2);
        virtual double total_volume();

        virtual bool contains_radius(double rad);
	    virtual bool contains_height(double ht);

        NeiloidStump(const Point& first, const Point& second);
        virtual ~NeiloidStump(){};

    public:

        Point first;            // lowest stem measure on the tree
        Point second;           // second stem measure

};



class ParaboloidStump : public Stump {

    public:

        virtual std::string print();
        virtual double radius(double height);
        virtual double height(double radius);
        virtual double volume(double r1, double r2);
        virtual double total_volume();

        virtual bool contains_radius(double rad);
	    virtual bool contains_height(double ht);

        ParaboloidStump(const Point& ground, const Point& stump, ParaboloidSection & base);
        virtual ~ParaboloidStump(){};

    public:

        ParaboloidSection base;    // ParaboloidSection defined by the first three measures

};




#endif