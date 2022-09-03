
#include "volpak.h"
#include <iostream>


#define DEBUG
#undef DEBUG


#ifdef DEBUG
#include <Rcpp.h>
#endif







Section::Section(Point first, Point second, Point third){

	this->first = first;
	this->second = second;
	this->third = third;

	this->base_section = false;


	// Set parameter values
	param();

}



/* Copy constructor */
Section::Section(const Section &rhs){
	
	this->first = rhs.first;
	this->second = rhs.second;
	this->third = rhs.third;

	this->a = rhs.a;
	this->p = rhs.p;
	this->q = rhs.q;

	this->base_section = rhs.base_section;

}



/* Assignment constructor */
Section& Section::operator=(const Section &rhs){

	this->first = rhs.first;
	this->second = rhs.second;
	this->third = rhs.third;

	this->a = rhs.a;
	this->p = rhs.p;
	this->q = rhs.q;

	this->base_section = rhs.base_section;

	return *this;

}



void Section::set_points(Point first, Point second, Point third){

	this->first = first;
	this->third = third;

	if (second.hag == -HUGE_VAL){
		this->second = midpoint(first, third);
	}
	else {
		this->second = second;
	}

	return;
}



void Section::set_base_section(){

	this->base_section = true;

	return;

}






void Section::param(){

    std::ostringstream msg;

	p = 0.0;
	q = 0.0;
	a = 0.0;

	double r1 = first.radius,  h1 = first.hag;
	double r2 = second.radius, h2 = second.hag;
	double r3 = third.radius,  h3 = third.hag;


	/* "Current Marc" would like "Future Marc" to know that he's really not a fan of this "lets make all the 
	derivatives positive numbers" approach */

	double rdif1 = r1 - r2;
	double rdif2 = r2 - r3;
	double t1 = (h2 - h1) / rdif1;
	double t2 = (h3 - h2) / rdif2;


	if (rdif1 < 0 || t1 <= 0.0){  
		msg << "Domain error in the first pair of measures" << std::endl;
		msg << "  r1 = " << r1 << "  r2 = " << r2 << std::endl;
		msg << "  h1 = " << h1 << "  h2 = " << h2 << std::endl;
		msg << "  t1 = " << t1 << std::endl;
		/* throw std::domain_error(msg.str()); */
		return;
	}


	if (rdif2 < 0 || t2 <= 0.0){  
		msg << "Domain error in the second pair of measures" << std::endl;
		msg << "  r2 = " << r2 << "  r3 = " << r3 << std::endl;
		msg << "  h2 = " << h2 << "  h3 = " << h3 << std::endl;
		msg << "  t2 = " << t2 << std::endl;
		/* throw std::domain_error(msg.str()); */
		return;
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
			}
			else	{		/* Parabola. */
				q = fabs((r1 - r3) / (a)); 
				p = ((h3 - h1) / (r1 - r3)) * q + (r1 + r3);
			}
		}
		else {
			a = 0.0;		/* Conical. */
		}

	}


	if(q < 0 || p < 0){
		msg << "Section::param: invalid parameter values:  q = " << q
		<< "  p = " << p
		<< std::endl;

		throw std::logic_error(msg.str());
	}


	this->a = a;
	this->p = p;
	this->q = q;


	return;
}





/*  Calculate the height of the frustum above the passed radius.
This height + the height of the radius = theoretical height
of the tree.
*/
double Section::length_above(double radius){

	std::ostringstream msg;

	if (!contains_radius(radius)){
		msg << "Section::length_above: radius not in section:  radius = " << radius
			<< "  first.radius = " << first.radius << "  third.radius = " << third.radius
			<< std::endl;

		throw std::invalid_argument(msg.str());
	}


	double x = 0.0;


	if (a < 0.0){				/* Concave Parabolic frustum. */

		if (q == 0.0){
			msg << "Section::length_above: Undefined length for radius " << radius << " when q == 0" << std::endl;
			throw std::domain_error(msg.str());
		}

		x = (p - radius) * radius / q;
	}
	else if (a > 0.0) {			/* Convex hyperbolic frustum. */

		if (q == radius){
			msg << "Section::length_above: Undefined legnth for radius " << radius << " when q == radius" << std::endl;
			throw std::domain_error(msg.str());
		}

		x = p * radius / (q - radius);
	}


	if (x < 0.0){
		x = 0.0;
	}


	return x;
}



double Section::height_when(double radius){
	
	double ratio = 0.0;
	double x = 0.0;
	double height = 0.0;

	if((a == 0) || ((a < 0) && (first.radius >= p/2))){

		ratio = (radius - third.radius) * (third.hag - first.hag) / (first.radius - third.radius);
		height = third.hag - ratio;

	} else {

		x = length_above(radius);
		height = length_above(first.radius) - x;

	}

	return height;

}



double Section::calcx(double ht){

	double length = length_above(first.radius) + first.hag;
	return length - ht;

}




double Section::radius_at(double ht){

    std::ostringstream msg;

	if(a == 0.0 && base_section){
		throw std::invalid_argument("Section::radius_at: called on base_section when a = 0.0\n");
	}

	double z, rad;
	double x = calcx(ht);


	if (a < 0.0){ 	/* Concave parabolic. */
		
		z = discriminant(ht);
		
		if (z < 0.0) {
		    /* msg << "Section::radius_at: z = " << z << "  ht = " << ht << "  x = " << calcx(ht) << std::endl;
			msg << "  a = " << a << "  p = " << p << "  q = " << q << std::endl;
			throw std::domain_error(msg.str()); */
			return -HUGE_VAL;
		}
		
		rad = (p - sqrt(z)) / 2.0;

	}
	else if (a > 0.0){	/* Convex hyperbolic. */
		
		rad = q * x / (x + p);

	} else if ((a == 0.0) && !base_section){
		
		double ratio = (first.radius - third.radius) / (third.hag - first.hag) * (ht - first.hag);
		rad = first.radius - ratio;

	}


	if (rad < 0.0){
	    msg << "Section::radius_at: ht = " << ht << "  rad = " << rad << "  first.radius = " << "  x = " << x << std::endl;
		msg << "  p = " << p << "  q = " << q << "  a = "  << a << std::endl;
		throw std::domain_error(msg.str());
	}


	return(rad);
}







Point Section::midpoint(Point base, Point top){

    std::ostringstream msg;

	double ratio = 0.0;
	double midR = 0.0;
	double midht = average(base.hag, top.hag);

	if(a == 0.0){
		ratio = (base.radius - top.radius) * (midht - base.hag) / (top.hag - base.hag);
		midR = base.radius - ratio;
	} else {
		midR = this->radius_at(midht);
	}


	return Point(midht, midR);
}





/* Formula obtained by volume of revolution using the shell method, 
although the disk method would have been just as valid.
*/
double Section::vhyper(double R, double r){

    std::ostringstream msg;
    
	if (R < r){
		std::swap(R, r);
	}


	double vol = 0.0;
	double s = q - r;
	double t = q - R;

	if ((s/t) > 0.0){
		vol = pow(R, 3.0) / t - pow(r, 3.0) / s + 
			(R - r) * (R + r + 2.0 * q) -
			2.0 * q * q * log(s/t);

		vol = M_PI * p * vol;
	}
	else {
		vol = -HUGE_VAL;
	}


	if (vol < 0.0){
	    msg << "Section::vhyper: R = " << R << "  r = " << r 
	        << "  p = " << p << "  q = " << q 
	        << "  vol = " << vol << std::endl;
	    
		throw std::domain_error(msg.str());
	}
		

	return vol;
}



/* Formula obtained by volume of revolution using the shell method, 
disk method not feasible in this case as the function expressing 
radius in terms of height is not invertible.
*/
double Section::vpara(double R, double r){

    std::ostringstream msg;
    
	if (R < r){
		std::swap(R, r);
	}


	double vol = p * (pow(R, 3.0) - pow(r, 3.0)) / 3.0 - 
		(pow(R, 4.0) - pow(r, 4.0)) / 2.0;

	vol = M_PI * vol / q;

	
	if (vol < 0.0){
	    msg << "Section::vpara: R = " << R << "  r = " << r 
         << "  p = " << p << "  q = " << q 
         << "  vol = " << vol << std::endl;
	    
		throw std::domain_error(msg.str());
	}
	
	
	return vol;
}




double Section::vcone(double R, double r){
	
	std::ostringstream msg;
	
	if (a != 0.0){
	    msg << "Section::vcone: Section is not conical (a = " << a << ")" << std::endl;
		throw std::invalid_argument(msg.str());
	}

	if (R < r){
		std::swap(R, r);
	}


	double taper = (third.hag - first.hag) / (first.radius - third.radius);
	double vol = (M_PI / 3.0) * abs(taper) * (pow(R, 3.0) - pow(r, 3.0));


	if (vol < 0.0){
	    msg << "Section::vcone: R = " << R << "  r = " << r 
         << "  p = " << p << "  q = " << q 
         << "  vol = " << vol << std::endl;
		throw std::domain_error(msg.str());
	}

	return vol;
}






double Section::conic_volume(double r1, double r2){
	
	double vol = 0.0;

	if (a < 0.0){
		vol = vpara(r1, r2);
	}
	else if (a > 0.0){
		vol = vhyper(r1, r2);
	}
	else {
		vol = vcone(r1, r2);
	}

	return vol;
}



double Section::newton_volume(Point p1, Point p2, Point p3){

	double vol = (M_PI / 6.0) * 
		(pow(p1.radius, 2.0) + 4.0 * pow(p2.radius, 2.0) + pow(p3.radius, 2.0)) * 
		(p3.hag - p1.hag);

	return vol;

}






double Section::volume(Point p1, Point p2){

	double vol = 0.0;

	vol = conic_volume(p1.radius, p2.radius);

	return vol;

}








double Section::total_volume(){

	double vol = 0.0;

#ifdef NEWTON

	vol = newton_volume(first, second, third);

# else

	vol = conic_volume(first.radius, third.radius);

#endif

	return vol;

}


