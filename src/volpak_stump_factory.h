/* Factory for creating relevant Stump. This class replaces much of what was vpakinit() and param() */


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

        std::unique_ptr<Stump> createStump(const Point& first, const Point& second, const Point& third);
        std::string printStumpType(StumpType type);

};


#endif