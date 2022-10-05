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

Volpak code used Newton's formula for log volumation, despite there being 
equations for volume of each type of stem section. Newton's formula is 
provided in the 'total_volume' method, while 'volume' provides the equations
derived for each conic.

C++ Notes: Section is a purely virtual class, obtained in practice by not 
    implementing methods for all virtual functions. This means that a Section
    cannot be instantiated. The base class does define a lot of functionality
    shared by all three derived classes.

    Factory class currently copies and modifies sections, in order to subdivide
    sections with three measures into subsections containing only two (and a 
    midpoint). Should strictly provide a method for updating data in class,
    allowing data to be protected by a 'private' access specifier; I have gone
    another direction. Member functions do not modify the class though, and are 
    thus marked as const.

*/



#ifndef SECTION_H
#define SECTION_H


#include <memory>


#include "volpak.h"
#include "volpak_point.h"



class Section {

public:

	// Methods shared with derived classes

    double total_volume() const;
    double calcx(double ht) const;
    bool contains_radius(double rad) const;
	bool contains_height(double ht) const;
    Point midpoint() const;
    double discriminant(double ht) const;


	// Virtual methods, different for each derived class

    virtual std::string print() const;
	virtual double radius(double ht) const = 0;
	virtual double height(double radius) const = 0;
    virtual double volume(double r1, double r2) const = 0;
    virtual double length_above(double radius) const = 0;


    // All the ways of making a Section
    Section();
	Section(const Point &  first, const Point &  second, const Point &  third, double & p, double & q);


    // Destructor must be virtual for the derived class destructors to be called
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

        virtual std::string print() const;
        virtual double radius(double ht) const;
        virtual double height(double radius) const;
        virtual double volume(double r1, double r2) const;
        virtual double length_above(double radius) const;


        // Destructor, apparently required for the derived class destructors to be called
        ConeSection() : Section() {};
        ConeSection(const Point &  first, const Point &  second, const Point &  third, double & p, double & q);
        virtual ~ConeSection(){};

};



class ParaboloidSection : public Section {

    public:

        virtual std::string print() const;
        virtual double volume(double r1, double r2) const;
        virtual double radius(double ht) const;
        virtual double height(double radius) const;
        virtual double length_above(double radius) const;


        // Destructor, apparently required for the derived class destructors to be called
        ParaboloidSection() : Section() {};
        ParaboloidSection(const Point &  first, const Point &  second, const Point &  third, double & p, double & q);
        virtual ~ParaboloidSection(){};

};



class HyperboloidSection : public Section {

    public:

        virtual std::string print() const;
        virtual double volume(double r1, double r2) const;
        virtual double radius(double ht) const;
        virtual double height(double radius) const;
        virtual double length_above(double radius) const;


        // Destructor, apparently required for the derived class destructors to be called
        HyperboloidSection() : Section() {};
        HyperboloidSection(const Point &  first, const Point &  second, const Point &  third, double & p, double & q);
        virtual ~HyperboloidSection(){};

};



#endif
