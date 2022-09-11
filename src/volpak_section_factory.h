/* Factory for creating relevant Section. This class replaces much of what was vpakinit() and param() */


#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>

#include "volpak.h"



#ifndef SECTIONFACTORY_H
#define SECTIONFACTORY_H


class SectionFactory {

    public:

        enum SectionType {
            Unknown,
            Paraboloid,
            Hyperboloid,
            Cone
        };

        std::unique_ptr<Section> createSection(const Point& first, const Point& second, const Point& third);
        std::vector<std::unique_ptr<Section>> splitSection(const Section& log);

};


#endif