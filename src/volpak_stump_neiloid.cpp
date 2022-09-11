/*  */



#include "volpak_stump.h"



/* NeiloidStump Methods:

Extrapolation of ground radius using newtons' method.
 
 Assuming a neiloid shape for the base of the tree:
 a * r^2 = ht^3
 
 Not knowing an appropriate value of 'a', newtons' method is
 used to estimate the rate of change in r^(2/3). 
 */


NeiloidStump::NeiloidStump(const Point& first, const Point& second){

    /* Can't do radius calc without initialising first and second */

    this->first = first;
    this->second = second;


    double rad = this->radius(0.0);

    Point tmpground(0.0, rad);
    ground = tmpground;
    
    
    if (ht > 0.0){
        rad = this->radius(ht);
    }
    else {
        rad = ground.radius;
    }
    
    
    if (rad < 0.0){
		throw std::domain_error("Tree::set_neiloid_stump: Neiloid model produced a stump radius < 0.0");
    }
    
    Point tmpstump(ht, rad);
    stump = tmpstump;


}



std::string NeiloidStump::print(){

    std::ostringstream msg;


    msg << "NeiloidStump: " << std::endl;
    msg << "\t" << "ground: " << ground.print() << std::endl;
    msg << "\t" << "stump: " << stump.print() << std::endl;
    msg << "\t" << "first: " << first.print() << std::endl;
    msg << "\t" << "second: " << second.print() << std::endl;


    return msg.str();

}





double NeiloidStump::radius(double ht){

    std::ostringstream msg;
    
    if (ht > second.hag){
        throw std::invalid_argument("NeiloidStump::radius: Neiloid model is not appropriate for points above the second measure\n");
    }
    
    Point mid12 = average(first, second);

    double twothirds = 2.0 / 3.0;
    double rtrans = pow(first.radius, twothirds);
    double dif = (rtrans - pow(mid12.radius, twothirds)) / (mid12.hag - first.hag);
    
    if (dif < 0.0){
        dif = 0.0;
    }
    
    double rad = pow(rtrans + dif * (first.hag - ht), 1.5);
    
    
    if (rad < 0.0){
		msg << "NeiloidStump::radius: stump radius < 0.0;  rad: " << rad << "  ht: " << ht << std::endl 
        msg << "\tdif = " << dif << "  rtrans = " << rtrans << std::endl;
        msg << this->print();
		throw std::domain_error(msg.str());
    }
    
    return rad;

}




double NeiloidStump::height(double rad){

	std::ostringstream msg;

    if (rad < second.radius){
        throw std::invalid_argument("NeiloidStump::height: Neiloid model is not appropriate for points above the second measure\n");
    }
    
    // For a neiloid, the height is proportional to radius^(2/3)
    double twothirds = 2.0 / 3.0;
    double r0Lin = pow(ground.radius, twothirds);
    double radiLin = pow(rad, twothirds);
    double ht = first.hag * (r0Lin - radiLin) / (r0Lin - pow(first.radius, twothirds));
    
    
    if (ht < 0.0){
		msg << "NeiloidStump::height: stump radius < 0.0;  rad: " << rad << "  ht: " << ht << std::endl 
        msg << this->print();
		throw std::domain_error(msg.str());
    }
    
    return ht;
}





/* Calculate the volume for a point below the first measure */

double NeiloidStump::volume(Point point){	
    
    double vol = 0.0;
	double radi = point.radius, ht = point.hag;
    double r0Lin = 0.0, radiLin = 0.0;
    double twothirds = 2.0 / 3.0;
    
    if (radi == 0.0 && ht == 0.0){
        return -HUGE_VAL;
    }
    
    /* Now either, a1 == 0 or (a1 < 0 && ground.radius >= p1 / 2.0) */

    // For a neiloid, the height is proportional to radius^(2/3)
    r0Lin = pow(ground.radius, twothirds);


    if (radi > 0.0){
        radiLin = pow(radi, twothirds);
    }
    else {
        /* Calculate what percentage of the length between the ground and the first measure is occupied by the given height.
            Scale the difference in radius^(2/3), between the ground and the first measure point, by the same amount.
            */
        radiLin = r0Lin - ht / first.hag * (r0Lin - pow(first.radius, twothirds));
    }


    if (ht == 0.0){
        /* Similar logic as per the missing radius; scale the height of the first measure based on the change in radius^(2/3)
            as a percentage of the total change between ground and the first measure.
            */
        ht = first.hag * (r0Lin - radiLin) / (r0Lin - pow(first.radius, twothirds));
    }


    /* Equation obtained from the volume of revolution of a neiloid, using the shell method (as per the equivalent calculation
        for a paraboloid and hyperboloid in Section::vpara and Section::vhyper, respectively.
        */
    vol = M_PI_4 * ht * (r0Lin * r0Lin + radiLin * radiLin) * (r0Lin + radiLin);
    return vol;

}



double NeiloidStump::vol_stump_to_first_meas(){

    return volume(first) - volume(stump);

}




bool NeiloidStump::contains_radius(double rad){

	return ((rad >= first.radius) && (rad <= ground.radius)) || (abs(ground.radius - rad) < RADTOL) || (abs(first.radius - rad) < RADTOL);

}




bool NeiloidStump::contains_height(double ht){

	return ((ht <= first.hag) && (ht >= ground.hag)) || (abs(first.hag - ht) < HTTOL) || (abs(ground.hag - ht) < HTTOL);

}
