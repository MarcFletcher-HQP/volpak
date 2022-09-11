/* Storage for a section of stem consisting of three measures

Sections of stem have a shape determined by the two measure points and
described by the parameter values 'a' (change in taper), 'p', and 'q'. The
value of 'a' determines whether the section is represented by a cone
(a == 0), a "concave" paraboloid (a < 0), or a "convex" hyperboloid (a > 0).

In the original volpak; each stem section had a locally defined position
along the stem, calculated with reference to the total length of the stem.
The stem length is extrapolated using the shape of the curve defining the
radius of the section, which is not expected to be the same for each 
section.

*/

#include <math.h>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>


#include "volpak.h"
#include "volpak_point.h"
#include "volpak_section_factory.h"



#ifndef SECTION_H
#define SECTION_H

class Section {

public:

	// Methods shared with derived classes

    std::string print();
    double total_volume();
    double calcx(double ht);
    bool contains_radius(double rad);
	bool contains_height(double ht);
    Point Section::midpoint(Point base, Point top)


	// Virtual methods, different for each derived class
    
	virtual double radius(double ht);
	virtual double height(double radius);
    virtual double volume(double r1, double r2);
    virtual double length_above(double radius);


	// Destructor, apparently required for the derived class destructors to be called
	Section(Point first, Point second, Point third, double p, double q);
    virtual ~Section(){};


public:

	Point first;
	Point second;
	Point third;
	
	double p = 0.0;
	double q = 0.0;

};




class ConeSection : public Section {

    public: 

        virtual double radius(double ht);
        virtual double height(double radius);
        virtual double volume(double r1, double r2);
        virtual double length_above(double radius);


        // Destructor, apparently required for the derived class destructors to be called
        ConeSection(Point first, Point second, Point third, double p, double q);
        virtual ~ConeSection(){};

};



class ParaboloidSection : public Section {

    public: 

        virtual double volume(double r1, double r2);
        virtual double radius(double ht);
        virtual double height(double radius);
        virtual double length_above(double radius);


        // Destructor, apparently required for the derived class destructors to be called
        ParaboloidSection(Point first, Point second, Point third, double p, double q);
        virtual ~ParaboloidSection(){};

};



class HyperboloidSection : public Section {

    public: 

        virtual double volume(double r1, double r2);
        virtual double radius(double ht);
        virtual double height(double radius);
        virtual double length_above(double radius);


        // Destructor, apparently required for the derived class destructors to be called
        HyperboloidSection(Point first, Point second, Point third, double p, double q);
        virtual ~HyperboloidSection(){};

};



#endif