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

        std::string printSectionType(SectionType type);
        SectionType getSectionType(std::unique_ptr<Section> & ptr);
        std::unique_ptr<Section> copySection(std::unique_ptr<Section> & ptr);
        std::unique_ptr<Section> createSection(const Point& first, const Point& second, const Point& third);
        std::unique_ptr<Section> subdivideSection(std::unique_ptr<Section> & ptr, const Point & base, const Point & top);

};


#endif
