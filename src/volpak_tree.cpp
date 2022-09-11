/* Coming to grips with Volpak
 
 Another painful episode.
 */


#include "volpak_tree.h"


#define DEBUG
#undef DEBUG

#ifdef DEBUG
#include <Rcpp.h>
#endif



Tree::Tree(const std::vector<double> &radii, const std::vector<double> &hts, const double &treeht, double stumpht){
    
    // Check arguments
    if (radii.size() != hts.size()){
        throw std::invalid_argument("Tree::Tree: Inconsistent length in height and diameter arguments.");
    }
    
    if (radii.size() < 2){
        throw std::invalid_argument("Tree::Tree: Require at least 2 measures, other than tree height.");
    }
    
    
    
    /*
     Stem measures and interpolated midpoints are stored together in the following pattern:
     [ground, meas1, mid12, meas2, ..., lastmeas, lastmid, top]
     
     If no tree height is provided then the lastmid and top are omitted.
     
     Given two measurements, the below code creates:
     2 elements (of the vectors) for measures,
     1 midpoint between measures,
     1 point below the first measure (ground or stump);
     for a total of 4 points.
     
     Each additional measure adds another 2 points, including tree height.
     */
    
    
    measures = radii.size();
    int numpts = measures;
    if (treeht > 0.0){
        numpts += 1;
    }
    

	/* rather than use std::vector::push_back, which potentially copies the existing array to 
	ensure contiguous storage, just allocate up-front.*/
    measpoints.resize(numpts, Point());
    
    
    // Set values for measure points
    for (int i = 0; i < measures; i++){
        Point point(hts[i], radii[i]);
        measpoints[i] = point;
    }
    
    
    // Create a pseudo measure for tree height
    if (treeht > 0.0){
        Point top(treeht, 0.0);
        measpoints.back() = top;
    }
    

    // Set stump height
    if (stumpht < 0.0){
        stumpht = STUMPHT;
    }
    
    
    
    /* Set ground and stump radius - do what for measures == 2? */

    std::unique_ptr<Stump> stump = StumpFactory::createStump(measpoints[0], measpoints[1], measpoints[2]);


    
    /* Create logs within a tree */

    if (measures == 2 && measures == numpts){	/* two measures and no tree height */
        
		Point mid12 = average(measpoints[0], measpoints[1]);
        Section log1(measpoints[0], mid12, measpoints[1]);
        sections.push_back(log1);

    }
    else {
        
		/* C++ Note: declare variables within the scope of the for-loop to allow the variable to be re-used on each iteration */

        for (int i = 0; i < numpts - 2; i++){

            /* Create mid-points and calculate mid-point radius using the shape of the coarser stem section. */

            std::unique_ptr<Section> coarse = SectionFactory::createSection(measpoints[i], measpoints[i + 1], measpoints[i + 2]);


            /* Create section from each half of the coarse representation */

            std::vector<std::unique_ptr<Section>> logs = SectionFactory::splitSection(coarse);


            if(sections.begin() != sections.end()){

                std::unique_ptr<Section> log12 = sections.back();
                log12->second = average(logs[0]->second, log12->second);

                sections.back() = log12;

            } else {

                sections.push_back(logs[0]);

            }

            sections.push_back(logs[1]);

        }
        
    }

    
}







double Tree::total_height(){
    
    double ht = 0.0;

    if (measpoints.size() == measures + 1){
        ht = (measpoints.back()).hag;
    }

    return ht;
}





bool Tree::check_totht(){
    
    /* cos(20.0) was the original volpak value: don't know if they really meant 20 radians, or if they intended 20 degrees. 
     as cos(20.0) ~= 0.34 it seems pretty unlikely that this condition would trigger, unlike when using 
     cos(20 * pi / 180) ~= 0.94, which aligns with the 6% required by the DST field manual.
     */
    bool response = true;
    if (total_height() > 0.0){
        response = total_height() < cos(20.0 * M_PI / 180) * (measpoints.back()).hag;
    }
    
    return response;
}









/* Height at which the tree has the given radius */
double Tree::height(double rad){			// formerly double htd(double d1);
    
	std::ostringstream msg;

    if (!ground.is_valid()){
        throw std::invalid_argument("Tree::height: Ground has not been initialised\n");
    }
    
    if (rad < 0.0){
		msg << "Tree::height: Supplied radius (" << rad << ") less than zero" << std::endl;
        throw std::invalid_argument(msg.str());
    }
    
    if (rad > (stump->ground).radius || rad < (measpoints.back()).radius){
		return -HUGE_VAL;
    }
    
    
    // Find the section with first.radius < rad < third.radius
    auto it = std::find_if(sections.begin(), sections.end(), 
		[rad](Section &elem){return elem->contains_radius(rad); });
    
    
    if (it == sections.end()){

        if (rad > measpoints[0].radius){     // Check whether radius is between ground and the first measure

            return stump->height(rad);

        } else {  // If the radius couldn't be found in the tree, then throw a tantrum

            msg << "Tree::height: Could not find radius (" << rad << ") in tree: " << std::endl; 
            msg << this->print() << std::endl;
            throw std::domain_error(msg.str());

        }

    }

    
    // Calculate the height corresponding to the radius in the given section
    double ht = it->height(rad);
    
    
    // If the returned height is greater than the tree height, then pitch a fit.
    if (ht > this->total_height()){
		msg << "Tree::height: Calculated height (" << ht << ") exceeds the tree height (" << total_height() << ")" << std::endl;
        throw std::domain_error(msg.str());
    }
    
    
    // If the returned height is not in the section, then cry about it.
    if (!(it->contains_height(ht))){
		msg << "Tree::height: Calculated height (" << ht << ") does not lie in section: [" << 
        it->first.hag << ", " << it->third.hag << "]" << "  ht = " << ht << std::endl;
        throw std::domain_error(msg.str());
    }
    
    
    return ht;
}




/* Calculate the radius of the tree at the given height above ground */
double Tree::radius(double ht){			// formerly double dht(double h1);
    
	std::ostringstream msg;

    if (!ground.is_valid()){
        throw std::invalid_argument("Tree::radius: Ground has not been initialised\n");
    }
    
    if (!stump.is_valid()){
        throw std::invalid_argument("Tree::radius: Stump has not been initialised\n");
    }
    
    if (ht < 0.0){
		msg << "Tree::radius: Supplied height (" << ht << ") less than zero" << std::endl;
		throw std::invalid_argument(msg.str());
    }
    
    if (ht > total_height()){
		return -HUGE_VAL;
    }
    
    
    // Check whether height is below the first measure; use neiloid calculation if so, otherwise interpolate between measures.
    double rad = 0.0;
    if (ht < measpoints[0].hag){

        return stump->radius(ht);

    }
    else {
        
        // Find the section with first.radius < rad < third.radius
        auto jt = std::find_if(sections.begin(), sections.end(),
                          [ht](Section &elem){return elem->contains_height(ht); });
        
        
        // If a section could not be found, then throw a wobbly.
        if (jt == sections.end()){
			msg << "Tree::radius: Stem section could not be located for height " << ht << std::endl;
            throw std::invalid_argument(msg.str());
        }
        
        
        // Calculate the radius for the corresponding height in the given section
        rad = jt->radius(ht);
        
        
        // If the returned radius is not in the section, then light the beacons and call for aid.
        if (!(jt->contains_radius(rad))){
			msg << "Tree::radius: Calculated radius (" << rad << ") does not lie in section:\n\tmin = " << jt->third.radius 
				<< "  max = " << jt->first.radius << "  ht = " << ht << std::endl;

			throw std::domain_error(msg.str());
        }
    }
    
    
    return rad;
}




/* Calculate the volume between the stump and the given height along the stem */
double Tree::volume_to_hag(double ht){      /* previously double volh(double) */
    
    if (!stump.is_valid()){
        throw std::invalid_argument("Tree::volume_to_hag: Stump has not been initialised\n");
    }
    
    if (!measpoints[0].is_valid()){
        throw std::invalid_argument("Tree::volume_to_hag: First measure point has not been initialised\n");
    }

    
    if (ht < 0.0 || ht > (total_height())){
        return -HUGE_VAL;
    }
    
    
    // Volume to ground level
    if (ht <= stump.hag){
        return 0.0;
    }
    
    
    // Volume of the stump, to be excluded from all subsequent volumes.
    double stumpvol = stump_vol(stump);
    
    
    // Volume of the entire tree, which there is already a function for.
    if (ht == (total_height())){
        return vol_above_stump();
    }
    
    
    
    if (ht < measpoints[0].hag){
		
        Point point(ht, 0.0);

        if(neiloid_stump){
            return neiloid_vol(point) - stumpvol;
        } else {
            return base.volume(ground, point) - stumpvol;
        }
    }
    
    
    // Calculate volume between the stump and the first measure
    double vol = stump_vol(measpoints[0]) - stumpvol;

    if (ht == measpoints[0].hag){
        return vol;
    }
    
    
    /* Accumulate volume of all sections below the given height 
     
     Volpak code used Newton's formula for log volumation, despite there being equations for 
     volume of each type of stem section. Code for using hyperboloid/paraboloid/conical volume
     equations are provided when compiled with DEBUG active.
     */
    
    auto last = std::find_if(sections.begin(), sections.end(),
                             [ht](Section &elem){return elem.contains_height(ht); });


    for (auto it = sections.begin(); it != last; it++){
        
        vol += it->total_volume();

        if (ht == it->third.hag){
            return vol;
        }

    }
    
    
    // Both methods use the same process for the final section.
    double rad = last->radius_at(ht);

    Point lastpoint(ht, rad);
    
    vol += last->volume(last->first, lastpoint);
    
    return vol;
}





/* Calculate the volume between the stump and the supplied radius */
double Tree::volume_to_radius(double rad) {			// formerly double vold(double d1)
    
    if (!stump.is_valid()){
        throw std::invalid_argument("Tree::volume_to_radius: Stump has not been initialised\n");
    }
    
    if (!measpoints[0].is_valid()){
        throw std::invalid_argument("Tree::volume_to_radius: First measure point has not been initialised\n");
    }

    double vol = 0.0;
    
    
    if (rad < 0.0 || rad > ground.radius){
        return -HUGE_VAL;
    }
    
    
    // Volume to ground level
    if (rad >= stump.radius){
        return 0.0;
    }
    
    
    // Volume of the stump, to be excluded from all subsequent volumes.
    double stumpvol = stump_vol(stump);
    
    
    // Volume of the entire tree, which there is already a function for.
    if (rad == 0.0){
        return vol_above_stump();
    }
    
    
    // radii greater than the first measure have their own function for volume.
    if (rad > measpoints[0].radius){
		
        Point point(0.0, rad);

        if(neiloid_stump){
            vol = neiloid_vol(point);
        } else {
            vol =  base.volume(ground, point);
        }

        return vol - stumpvol;
        
    }
    
    
    // Calculate volume between the stump and the first measure
    vol = stump_vol(measpoints[0]) - stumpvol;

    if (rad == measpoints[0].radius){
        return vol;
    }
    
    
    
    /* Accumulate volume of all sections below the given height
     
     Volpak code used Newton's formula for log volumation, despite there being equations for
     volume of each type of stem section. Code for using hyperboloid/paraboloid/conical volume
     equations are provided when compiled with DEBUG active.
     */
    
    auto last = std::find_if(sections.begin(), sections.end(),
                             [rad](Section &elem){return elem.contains_radius(rad); });


#ifdef DEBUG
    Rprintf("Tree::volume_to_radius:  i: 0  vol: %.6f\n", vol);
#endif

    
    for (auto it = sections.begin(); it != last; it++){

        vol += it->total_volume();


#ifdef DEBUG
    Rprintf("Tree::volume_to_radius:  i: %i  vol: %.6f\n", std::distance(sections.begin(), it)+1, vol);
#endif

        if (rad == it->third.radius){
            return vol;
        }
    }
    
    
    // Both methods use the same process for the final section.
    
    Point lastpoint(last->height_when(rad), rad);
    
    vol += last->volume(last->first, lastpoint);

    #ifdef DEBUG
    Rprintf("Tree::volume_to_radius:  i: %i  vol: %.6f\n", std::distance(sections.begin(), last), vol);
    Rprintf("Tree::volume_to_radius:  section 'last': \na: %.6f  p: %.6f  q:%.6f\nfirst: (%.2f, %.6f) \nsecond: (%.2f, %.6f) \nthird: (%.2f, %.6f)\n", 
        last->a, last->p, last->q, 
        last->first.hag, last->first.radius,
        last->second.hag, last->second.radius,
        last->third.hag, last->third.radius
    );
    Rprintf("Tree::volume_to_hag:  lastpoint: (%.2f, %.6f)  vol: %.6f\n", lastpoint.hag, lastpoint.radius, last->volume(last->first, lastpoint));
#endif
    
    return vol;	
}





/* fletchm 2021-04-22: Extrapolating tree height using newton's method
 
 Estimate the taper, and change in taper, at each point using a finite difference approximation.
 
 Varying degrees of taylor approximation are used to estimate the tree height.
 
 Multiple equations have been added, a dataset of bole length measurements will determine the most
 accurate.
 */
double Tree::missing_height(int method){
    
	std::ostringstream msg;

    double tht = 0.0;
    double taper = 0.0;
    double Dtaper = 0.0;
    double discriminant = 0.0;
    
    
    Section last = sections.back();
    
    if (last.a != 0.0){
        return last.length_above(last.third.radius);
    }
    
    
    /* check top three points for unfavourable zeros and fill with the same method? */
    double dh1 = last.third.hag - last.second.hag;
    double dh2 = last.second.hag - last.first.hag;
    
    
    
    switch (method){
    
    /* First order: forward difference */
    case 0:
        
        taper = (last.third.radius - last.second.radius) / dh1;
        tht = last.third.hag - (last.third.radius / taper);
        break;
        
        
        /* Second order: central difference */
    case 1:
        
        tht = last.third.hag - last.third.radius * dh2 / (last.second.radius - last.first.radius);
        break;
        
        
        /* Second order: forward difference */
    case 2:
        
        taper = 200 * (last.third.radius - last.second.radius) / dh1;
        Dtaper = last.third.radius / dh1 - last.second.radius * (dh1 + dh2) / (dh1 * dh2) + last.first.radius / dh2;
        Dtaper = 200 * Dtaper / dh1;
        
        discriminant = pow(taper, 2) - 4 * Dtaper * 200 * last.third.radius;		// modify this to contain diam.
        if (discriminant < 0.0){
			msg << "Tree::missing_height: negative discriminant last.third.radius: " << last.third.radius << "  taper: " << taper 
				<< "  Dtaper: " << Dtaper << std::endl;
            
			throw std::domain_error(msg.str());
        }
        
        tht = last.third.hag + (-taper - sqrt(discriminant)) / (2 * Dtaper);
        break;
        
        
    default:
        
        return -HUGE_VAL;
    
    }
    
    
    if (tht < 0.0){
		msg << "Tree::missing_height: Negative tree height " << tht << std::endl;
        throw std::domain_error(msg.str());
    }
    
    
    return tht;
}



