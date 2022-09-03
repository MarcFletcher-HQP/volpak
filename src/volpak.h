
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <iterator>
#include <exception>
#include <algorithm>
#include <string>
#include <math.h>



#define NEWTON
//#undef NEWTON



enum class UNIT {M, CM};


UNIT string_to_unit(std::string unitstr);
std::string unit_to_string(UNIT unit);


static double HTTOL = 0.1;
static double RADTOL = 0.05/200;




/* Storage for a single measure or pseudo-measure (midpoint, ground, stump, or top) */
class Point{

public:
	
	Point();
	Point(double ht, double rad);
	Point(const Point &rhs);
	Point& operator=(const Point &rhs);

	bool is_valid();

	bool below(const Point &point);
	bool above(const Point &point);


public:

	double hag = 0.0;
	double radius = 0.0;

};






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
class Section {

public:

	// Test-function for filling in missing tree height
	double missing_height(int method);



	// Volume calculations
	double vhyper(double r1, double r2);
	double vpara(double r1, double r2);
	double vcone(double r1, double r2);
	double conic_volume(double r1, double r2);
	double newton_volume(Point p1, Point p2, Point p3);
	double volume(Point p1, Point p2);
	double total_volume();



	// Quantities derived from parameters
	double discriminant(double ht){
		return p * p - 4 * q * calcx(ht);
	}

	double calcx(double ht);



	// Check whether points are contained in the section
	bool contains_radius(double rad){

		return ((rad >= third.radius) && (rad <= first.radius)) || (abs(first.radius - rad) < RADTOL) || (abs(third.radius - rad) < RADTOL);

	}

	bool contains_height(double ht){

		return ((ht <= third.hag) && (ht >= first.hag)) || (abs(third.hag - ht) < HTTOL) || (abs(first.hag - ht) < HTTOL);

	}
	
	

	// Derive points from section
	Point midpoint(Point base, Point top);
	double radius_at(double ht);
	double length_above(double radius);
	double height_when(double radius);



	// Replace points in the section, usually used after copying another section.
	void set_points(Point first, Point second, Point third);
	void set_base_section();



	// Calculate shape parameters for section
	void param();



	// Constructors
	Section(){};
	Section(const Section &rhs);
	Section& operator=(const Section &rhs);
	Section(Point first, Point second,
		Point third);
	Section(std::vector<double> diams, std::vector<double> hts);


public:

	Point first;
	Point second;
	Point third;
	
	double a = 0.0;
	double p = 0.0;
	double q = 0.0;

	bool base_section = false;

};






/* Storage for all measures associated with a tree

Contains:
Measure points (measpoints): points of measured radius and height above ground (hag).
Mid-points (midpoints): points whose radius has been inferred from the shape of the
surrounding log.
Sections (sections):	overlapping representations of the stem, defined by three
consecutive measures.
*/
class Tree {

public:

	double radius_at(double ht);
	double height_when(double rad);
	double volume_to_hag(double ht);
	double volume_to_radius(double rad);
	double missing_height(int method);
	
	double stump_ht();
	double stump_vol(Point point);
	double vol_above_stump();

	bool check_totht();
	double total_height();

	void set_neiloid_stump(double ht);
	double neiloid_radius(double ht);
	double neiloid_height(double rad);
	double neiloid_vol(Point point);

	Point first_point_above_height(double ht);

	Tree(const std::vector<double> &diams, const std::vector<double> &hts, const double &treeht, double stumpht);


public:

	unsigned int measures;
	bool neiloid_stump;

	Point ground;
	Point stump;
	std::vector<Point> measpoints;			// Note: measpoints aren't stored contiguously in memory, only the pointers; is std::vector beneficial at all?
	std::vector<Section> sections;
	Section base;

};





/* double average(double low, double high); */
double average(double low, double high);
Point average(Point, Point); 		// Surely this could be a template (combinations of std::shared_ptr<Point> and Point)

