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

	std::string print() const;
	double radius(double ht) const;
	double height(double rad) const;
	double volume_to_height(double ht, bool abovestump) const;
	double volume_to_radius(double rad, bool abovestump) const;
	
	double stump_vol() const;
	double stump_radius() const;
	double stump_height() const;
	double ground_radius() const;

    double vol_to_first_measure() const;
	double total_volume() const;

	bool check_totht() const;

	Point first_measure() const;
	Point last_measure() const;

	/* std::vector<std::unique_ptr<Section>>::iterator section_containing_height(double ht) const;
	std::vector<std::unique_ptr<Section>>::iterator section_containing_radius(double rad) const; */
	auto section_containing_height(double ht) const;
	auto section_containing_radius(double rad) const;

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