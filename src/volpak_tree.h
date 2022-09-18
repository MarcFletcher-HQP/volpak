/* Storage for all measures associated with a tree

Contains:
Measure points (measpoints): points of measured radius and height above ground (hag).
Mid-points (midpoints): points whose radius has been inferred from the shape of the
surrounding log.
Sections (sections):	overlapping representations of the stem, defined by three
consecutive measures.
*/



#include <memory>
#include <utility>
#include <algorithm>


#include "volpak.h"
#include "volpak_point.h"
#include "volpak_section.h"
#include "volpak_stump_factory.h"
#include "volpak_section_factory.h"


#ifndef TREE_H
#define TREE_H

class Tree {

public:

	std::string print();
	double radius(double ht);
	double height(double rad);
	double volume_to_height(double ht, bool abovestump);
	double volume_to_radius(double rad, bool abovestump);
	
	double stump_vol();
	double stump_radius();
	double stump_height();
	double ground_radius();

    double vol_to_first_measure();
	double total_volume();

	bool check_totht();

	Point first_measure();
	Point last_measure();

	Tree(const std::vector<double> &diams, const std::vector<double> &hts, const double &treeht, double stumpht);
	Tree(const Tree & t) = delete;
	Tree& operator=(const Tree & t) = delete;
	~Tree() = default;

public:

    double treeht;

	std::unique_ptr<Stump> stump;
	std::vector<std::unique_ptr<Section>> sections;

};

#endif