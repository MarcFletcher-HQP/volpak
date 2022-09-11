/*  */



#include "volpak_stump_factory.h"
#include "volpak_section_factory.h"



std::unique_ptr<Stump> StumpFactory::createStump(const Point& first, const Point& second, const Point& third){


    double stumprad = 0.0;
    double groundrad = 0.0;


    /* Get the shape of the section related to the first three measures */

    std::unique_ptr<Section> base = SectionFactory::createSection(first, second, third);


    if (base->type == Paraboloid){

        groundrad = base->radius(0.0);
        stumptype = ParaboloidStump;

        if (STUMPHT == 0.0){
            stumprad = groundrad;
        }
        else if (base->discriminant(0.0) >= 0.0 &&
                 groundrad >= base->first.radius &&
                 groundrad < (base->p / 2)){

                stumprad = base->radius(STUMPHT);
        }
        else {
            stumptype = NeiloidStump;
        }
    }
    else {
        stumptype = NeiloidStump;
    }


    switch(stumptype){

        case ParaboloidStump:
            Point ground(0.0, groundrad);
            Point stump(STUMPHT, stumprad);
            return std::make_shared<ParaboloidStump>(ground, stump, base);

        case NeiloidStump:
            return std::make_shared<NeiloidStump>(first, second);

        case Unknown:
            return nullptr;
        
        case default:
            return nullptr;

    }

}