/* Factory for creating relevant Section. This class replaces much of what was vpakinit() and param() */



#include "volpak_section_factory.h"


#define DEBUG
#undef DEBUG

#ifdef DEBUG
#include <Rcpp.h>
#endif



std::unique_ptr<Section> SectionFactory::createSection(const Point& first, const Point& second, const Point& third){

  std::ostringstream msg;


    SectionType sectiontype = Unknown;

	double p = 0.0;
	double q = 0.0;
	double a = 0.0;

	double r1 = first.radius;
	double r2 = second.radius;
	double r3 = third.radius;

	double h1 = first.hag;
	double h2 = second.hag;
	double h3 = third.hag;

	double rdif1 = r1 - r2;
	double rdif2 = r2 - r3;
	double t1 = (h2 - h1) / rdif1;
	double t2 = (h3 - h2) / rdif2;


	if (rdif1 < 0 || t1 <= 0.0){
        sectiontype = Cone;
        return std::make_unique<ConeSection>(first, second, third, p, q);
	}


	if (rdif2 < 0 || t2 <= 0.0){
        sectiontype = Cone;
		return std::make_unique<ConeSection>(first, second, third, p, q);
	}



	/* Difference in taper between end-points:

	If the taper changes by less than 1 cm/m (diameter/length) then the taper is considered
	fixed between the two sections, i.e. the frustum of a cone is used to interpolate
	between measures.

	Confusingly, the taper has been expressed in volpak as the number of metres of stem length
	for each centimetre change in diameter; so, bigger values imply less taper. With this
	in mind, if a > 0 then there is less taper at the lower measure (represented by 't1')
	and the taper increases along the section. Conversely if a < 0, then the taper
	decreases along the stem section.

	Dividing a stem into a neiloid stump, paraboloid mid-section and a conical top; the two
	cases described are likely to occur at the extremes of the paraboloid section. Where
	the stump transitions into the first log the taper can be quite high, decreasing rapidly
	over the first log; this corresponds to the case where a < 0 and the mid-point diameter
	is interpolated using the "concave parabola" equation. Likewise, as the merchantable
	section of the stem reaches its end the taper begins to increase with height, as the
	stem begins to taper to a point; this corresponds to the case where a > 0 and the mid-
	point diameter is interpolated using the "convex hyperbola" equation.

	The convex hyperbola equation is an implicit equation of the form:
	x * r - Q * x + P * r = 0.

	The concave parabola equation, on the other hand, takes the form:
	r^2 - P * r + Q * x  = 0.

	Where 'x' is the distance from the top of the tree (TotalHt - HAG) and P & Q are
	coefficients calculated in this function (hence why it was originally named 'param').

	In the parabolic case the coefficients have the same impact as for any quadratic, made
	complicated by a scale factor 'Q'. 'P/Q' is the taper at the tip of the tree, which has to
	be estimated from the measures available in the lower section of the stem. '2/Q' doubles as
	an estimate of the rate at which the taper is changing along the paraboloid.

	The expression for P can be obtained by differentiation of 'x' w.r.t. the radius and
	expressing the derivative in terms of the central finite difference, as well as describing
	the radius as being approximately the average of the two end points. The expression for Q
	is obtained by obtaining a finite difference approximation for the second derivative.

	In the hyperbolic case the parameters have a different interpretation. 'Q' is the maximum
	possible radius, atained at the base of an infinitely tall tree (absurd, right). The value
	of 'P' represents the position along the stem where the radius reaches half of the maximum.
	The expression used to estimate 'P' is obtained using a similar method for the Parabolic
	case, however a derivation of 'Q' escapes the author.
	*/

	if(rdif1 > 0.001 && rdif2 > 0.001 && t1 < 500 && t2 < 500){

		a = t1 - t2;

		if (fabs(a) >= 1 && a > -300){
			if (a >= 0.0) {	/* Hyperbola. */

                q = (t1 * r1 - t2 * r3) / (a);
				p = (h3 - h1) / (r1 - r3) * (q - r1) * (q - r3) / (q);

                sectiontype = Hyperboloid;

			}
			else	{		/* Parabola. */

                q = fabs((r1 - r3) / (a));
				p = ((h3 - h1) / (r1 - r3)) * q + (r1 + r3);

                sectiontype = Paraboloid;

			}
		}
		else {

            a = 0.0;		/* Conical. */

            sectiontype = Cone;

		}

	} else {

		sectiontype = Cone;

	}


	if(q < 0 || p < 0){
		msg << "Section::param: invalid parameter values:  q = " << q
		<< "  p = " << p
		<< std::endl;

		throw std::logic_error(msg.str());
	}


	/* The taper of a paraboloid switches sign, from increasing to decreasing with height,
	when r > p/2. In this instance, the original volpak code reverts to a cone for
	interpolation.*/

/* 	if ((sectiontype == Paraboloid) && (r1 >= p/2)){

		sectiontype = Cone;
		p = 0.0;
		q = 0.0;

	} */


    switch(sectiontype){

        case Paraboloid:
            return std::make_unique<ParaboloidSection>(first, second, third, p, q);

        case Hyperboloid:
            return std::make_unique<HyperboloidSection>(first, second, third, p, q);

        case Cone:
            return std::make_unique<ConeSection>(first, second, third, p, q);

        case Unknown:
            return nullptr;

        default:
            return nullptr;

    }


}





SectionFactory::SectionType SectionFactory::getSectionType(std::unique_ptr<Section> & ptr){

	SectionType type = Unknown;

	if(dynamic_cast<ParaboloidSection*>(ptr.get()) != NULL){

		type = Paraboloid;

	} else if (dynamic_cast<HyperboloidSection*>(ptr.get()) != NULL){

		type = Hyperboloid;

	} else if (dynamic_cast<ConeSection*>(ptr.get()) != NULL){

		type = Cone;

	}

	return type;

}




std::string SectionFactory::printSectionType(SectionFactory::SectionType type){

	switch(type){

		case Hyperboloid:
			return "Hyperboloid";

		case Paraboloid:
			return "Paraboloid";

		case Cone:
			return "Cone";

		case Unknown:
			return "Unknown";

		default:
			return "No Type Information";

	}

}




/* Copy a Section */

std::unique_ptr<Section> SectionFactory::copySection(std::unique_ptr<Section> & ptr){

	/* Get the type */

	SectionType type = getSectionType(ptr);

	switch(type){

		case Hyperboloid:
			return std::move(std::make_unique<HyperboloidSection>(*dynamic_cast<HyperboloidSection*>(ptr.get())));

		case Paraboloid:
			return std::move(std::make_unique<ParaboloidSection>(*dynamic_cast<ParaboloidSection*>(ptr.get())));
		
		case Cone:
			return std::move(std::make_unique<ConeSection>(*dynamic_cast<ConeSection*>(ptr.get())));

		case Unknown:
			return nullptr;
		
		default:
			return nullptr;

	}

}




/* Create sub-divide existing section */

std::unique_ptr<Section> SectionFactory::subdivideSection(std::unique_ptr<Section> & ptr, const Point & base, const Point & top){

	std::ostringstream msg;

#ifdef DEBUG
	Rcpp::Rcout << "SectionFactory::subdivideSection: Input section: " << std::endl;
	Rcpp::Rcout << ptr->print() << std::endl;
#endif

	if(!ptr->contains_height(base.hag) || !ptr->contains_radius(base.radius)){

		msg << "SectionFactory::subdivideSection: 'base' not contained in section" << std::endl;
		msg << ptr->print() << std::endl;
		msg << "base: " << base.print() << std::endl;

		throw std::invalid_argument(msg.str());

	}


	if(!ptr->contains_height(top.hag) || !ptr->contains_radius(top.radius)){

		msg << "SectionFactory::subdivideSection: 'top' not contained in section" << std::endl;
		msg << ptr->print() << std::endl;
		msg << "top: " << top.print() << std::endl;

		throw std::invalid_argument(msg.str());

	}


	/* Copy the section */

	SectionType type = getSectionType(ptr);
	std::unique_ptr<Section> newsection = copySection(ptr);


	/* Replace the base and top of the section */

	newsection->first = base;
	newsection->third = top;


	/* Calculate the new mid-point */

	newsection->second = newsection->midpoint();


	/* Create (yet another) section */

	return createSection(newsection->first, newsection->second, newsection->third);

}




