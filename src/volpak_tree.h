/* Tree a union of non-overlapping sections, of different shapes, between measured points.

Only class that should be used in external code.

C++ Note: After the constructor is called, none of the member functions modify the data 
	stored in the class. The use of the 'const' keyword following the function declaration
	indicates such to the compiler, which will throw an error if an attempt at modifying 
	the members is carried out.

	const is used elsewhere to generate compiler errors. Use of access specifier 'private'
	is used so that data members of 'Tree' are not accessable elsewhere.
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

	Tree(const std::vector<double> &diams, const std::vector<double> &hts, const double &treeht, double stumpht);
	Tree(const Tree & t) = delete; 				// Cannot copy tree
	Tree& operator=(const Tree & t) = delete;	// Cannot copy tree
	~Tree() = default;

private:

    double treeht;

	std::unique_ptr<const Stump> stump;  // Cannot modify object pointed to by 'stump' once added to Tree (compiler error).
	std::vector<std::unique_ptr<const Section>> sections; // Cannot modify Sections, pointed by elements of sections, once added to vector (compiler error).

};

#endif