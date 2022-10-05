/* Storage for a single measure or pseudo-measure (midpoint, ground, stump, or top) 

C++ Note: After the constructor is called, none of the member functions modify the data 
	stored in the class. The use of the 'const' keyword following the function declaration
	indicates such to the compiler, which will throw an error if an attempt at modifying 
	the members is carried out.

*/


#ifndef POINT_H
#define POINT_H



#include "volpak.h"




class Point{

public:

	Point();
	Point(double ht, double rad);

	bool is_valid() const;

	bool below(const Point &point) const;
	bool above(const Point &point) const;

	std::string print() const;


public:

	double hag = 0.0;
	double radius = 0.0;

};



Point average(Point, Point);
Point midpoint(Point base, Point top);



#endif
