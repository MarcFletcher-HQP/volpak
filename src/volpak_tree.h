/* Storage for all measures associated with a tree

Contains:
Measure points (measpoints): points of measured radius and height above ground (hag).
Mid-points (midpoints): points whose radius has been inferred from the shape of the
surrounding log.
Sections (sections):	overlapping representations of the stem, defined by three
consecutive measures.
*/


#include <math.h>
#include <utility>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>


#include "volpak.h"
#include "volpak_point.h"
#include "volpak_section.h"
#include "volpak_stump_factory.h"
#include "volpak_section_factory.h"


#ifndef TREE_H
#define TREE_H

class Tree {

public:

	double radius(double ht);
	double height(double rad);
	double volume_to_height(double ht);
	double volume_to_radius(double rad);
	
	double stump_ht();
	double stump_vol();
    double vol_to_first_measure();
	double vol_above_stump();

	bool check_totht();
	double total_height();

	void set_stump(double ht);

	Tree(const std::vector<double> &diams, const std::vector<double> &hts, const double &treeht, double stumpht);


public:

    bool treeht_measured;

	Stump* stump;
	std::vector<Point> measpoints;
	std::vector<Section*> sections;

};

#endif