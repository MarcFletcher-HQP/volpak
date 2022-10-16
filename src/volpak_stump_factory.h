/* Factory for creating relevant Stump. This class replaces much of what was vpakinit() and nloid() 

C++ Note: The 'Factory' design pattern separates the logic involved with creating an object, from 
    what the object does.
*/


#include <memory>

#include "volpak.h"
#include "volpak_stump.h"


#ifndef STUMPFACTORY_H
#define STUMPFACTORY_H


class StumpFactory {

    public:

        enum StumpType {
            Unknown,
            Neiloid,
            Paraboloid
        };

        std::string printStumpType(StumpType type);
        StumpType getStumpType(const std::unique_ptr<Stump> & ptr);
        std::unique_ptr<Stump> createStump(const Point& first, const Point& second, const Point& third);

};


#endif