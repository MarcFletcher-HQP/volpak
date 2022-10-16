/* Tree a union of non-overlapping sections, of different shapes, between measured points.

Only class that should be used in external code.

C++ Note: After the constructor is called, none of the member functions modify the data 
	stored in the class. The use of the 'const' keyword following the function declaration
	indicates such to the compiler, which will throw an error if an attempt at modifying 
	the members is carried out.

	Once an instance of a Tree is created there are no operations that should alter the 
	stump, or any of the sections. Use of const specifier following member function 
	declaration declares, to the compiler, that the member function will not alter the 
	class. Further, pointers to the stump and sections are marked with the 'private'
	access specifier, masking them to non-member functions. Unfortunately, the storage 
	pointed to can still be modified, this could be fixed by marking it as 'const' as 
	well, but this caused a lot of other problems.

	Marking a member function as const results in 'const this' rather than 'this' being
	passed to the member function, which may cause problems if the input is passed to
	subsequent non-const functions.

	Arguably this code would be simpler in many places if the author used shared_ptr,
	rather than unique_ptr. However, the challenge of using unique_ptr has hopefully
	resulted in code that avoids unnecessary copies.

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
	
	std::string stump_type() const;
	double stump_vol() const;
	double stump_radius() const;
	double stump_height() const;
	double ground_radius() const;

    double vol_to_first_measure() const;
	double total_volume() const;

	bool check_totht() const;
	double get_treeht() const;

	Point first_measure() const;
	Point last_measure() const;

	std::vector<std::unique_ptr<Section>>::const_iterator sections_begin() const;
	std::vector<std::unique_ptr<Section>>::const_iterator sections_end() const;

	Tree(const std::vector<double> &diams, const std::vector<double> &hts, const double &treeht, double stumpht);
	Tree(const Tree & t) = delete; 				// Cannot copy tree
	Tree& operator=(const Tree & t) = delete;	// Cannot copy tree
	~Tree() = default;

private:

    double treeht;

	std::unique_ptr<Stump> stump; 
	std::vector<std::unique_ptr<Section>> sections;

};

#endif