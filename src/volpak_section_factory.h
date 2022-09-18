/* Factory for creating relevant Section. This class replaces much of what was vpakinit() and param() */



#ifndef SECTIONFACTORY_H
#define SECTIONFACTORY_H



#include <memory>
#include <vector>

#include "volpak.h"
#include "volpak_section.h"




class SectionFactory {

    public:

        enum SectionType {
            Unknown,
            Paraboloid,
            Hyperboloid,
            Cone
        };

        SectionType getSectionType(std::unique_ptr<Section> & ptr);
        std::unique_ptr<Section> createSection(const Point& first, const Point& second, const Point& third);
        /* std::vector<std::unique_ptr<Section>> splitSection(std::unique_ptr<Section> log); */

};


#endif
