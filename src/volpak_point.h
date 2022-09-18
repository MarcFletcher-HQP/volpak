/* Storage for a single measure or pseudo-measure (midpoint, ground, stump, or top) */


#ifndef POINT_H
#define POINT_H



#include "volpak.h"




class Point{

public:

	Point();
	Point(double ht, double rad);
/* 	Point(const Point &rhs);
	Point& operator=(const Point &rhs); */

	bool is_valid();

	bool below(const Point &point);
	bool above(const Point &point);

	std::string print();


public:

	double hag = 0.0;
	double radius = 0.0;

};



Point average(Point, Point);
Point midpoint(Point base, Point top);



#endif