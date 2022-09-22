/*  */



#include "volpak_stump_factory.h"
#include "volpak_section_factory.h"



std::unique_ptr<Stump> StumpFactory::createStump(const Point& first, const Point& second, const Point& third){


    double stumprad = 0.0;
    double groundrad = 0.0;
    StumpType stumptype;



    /* Get the shape of the section related to the first three measures */
    SectionFactory factory;

    std::unique_ptr<Section> base = factory.createSection(first, second, third);


    if (factory.getSectionType(base) == SectionFactory::Paraboloid){

        groundrad = base->radius(0.0);
        stumptype = Paraboloid;

        if (STUMPHT == 0.0){
            stumprad = groundrad;
        }
        else if (base->discriminant(0.0) >= 0.0 &&
                 groundrad >= base->first.radius &&
                 groundrad < (base->p / 2)){

                stumprad = base->radius(STUMPHT);
        }
        else {
            stumptype = Neiloid;
        }
    }
    else {
        stumptype = Neiloid;
    }


    switch(stumptype){

        case Paraboloid:
        {
            const Point ground(0.0, groundrad);
            const Point stump(STUMPHT, stumprad);
            return std::make_unique<ParaboloidStump>(ground, stump, dynamic_cast<ParaboloidSection&>(*base));
        }
        case Neiloid:
        {
            const Point mid12 = base->midpoint(first, second);
            return std::make_unique<NeiloidStump>(first, mid12);
        }
        case Unknown:
        {
            return nullptr;
        }
        default:
        {
            return nullptr;
        }


    }

}
