/*  */

/* Stump class - extrapolation of the first three measure points to the base of the stem. Can either
be a ParaboloidSection, or a Neiloid. 


C++ Note: Stump is a pure virtual class, i.e. there can be no instances of a Stump, only of the 
    derived classes.

*/




#include <memory>


#include "volpak.h"
#include "volpak_point.h"
#include "volpak_section.h"



#ifndef STUMP_H
#define STUMP_H


class Stump {

    public:

        double stump_vol() const;
        

        Stump();
        Stump(const Point & ground, const Point & stump);

        virtual std::string print() const = 0;
        virtual double radius(double height) const = 0;
        virtual double height(double radius) const = 0;
        virtual double volume(double r1, double r2) const = 0;
        virtual double total_volume() const = 0;

        virtual bool contains_radius(double rad) const = 0;
	    virtual bool contains_height(double ht) const = 0;

        virtual ~Stump(){};


    public:

        Point ground;              // as written on the can
        Point stump;               // top of the stump

};




class NeiloidStump : public Stump {

    public:

        virtual std::string print() const;
        virtual double radius(double height) const;
        virtual double height(double radius) const;
        virtual double volume(double r1, double r2) const;
        virtual double total_volume() const;

        virtual bool contains_radius(double rad) const;
	    virtual bool contains_height(double ht) const;

        NeiloidStump(const Point& first, const Point& second);
        virtual ~NeiloidStump(){};

    public:

        Point first;            // lowest stem measure on the tree
        Point second;           // second stem measure

};



class ParaboloidStump : public Stump {

    public:

        virtual std::string print() const;
        virtual double radius(double height) const;
        virtual double height(double radius) const;
        virtual double volume(double r1, double r2) const;
        virtual double total_volume() const;

        virtual bool contains_radius(double rad) const;
	    virtual bool contains_height(double ht) const;

        ParaboloidStump(const Point& ground, const Point& stump, ParaboloidSection & base);
        virtual ~ParaboloidStump(){};

    public:

        ParaboloidSection base;    // ParaboloidSection defined by the first three measures

};




#endif