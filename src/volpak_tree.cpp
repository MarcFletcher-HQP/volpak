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

    std::ostringstream msg;


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


	/* rather than use std::vector::push_back, which potentially copies the existing array to
	ensure contiguous storage, just allocate up-front.*/

	int numpts = radii.size() + (int) (treeht > 0);

    std::vector<Point> measpoints(numpts);


    // Set values for measure points
    for (int i = 0; i < radii.size(); i++){

        Point point(hts[i], radii[i]);

        measpoints[i] = point;

    }


    // Create a pseudo measure for tree height

    this->treeht = 0.0;

    if (treeht > 0.0){

        this->treeht = treeht;

        Point top(treeht, 0.0);

        measpoints[numpts-1] = top;

    }


    // Set stump height
    if (stumpht < 0.0){

        stumpht = STUMPHT;

    }



    /* Set ground and stump radius - do what for measures == 2? */

    StumpFactory stump_factory;
    stump = stump_factory.createStump(measpoints[0], measpoints[1], measpoints[2]);




    /* Create logs within a tree */

    SectionFactory section_factory;

    if (measpoints.size() == 2 && treeht <= 0.0){	/* two measures and no tree height */

		Point mid12 = average(measpoints[0], measpoints[1]);

        std::unique_ptr<Section> log1 = section_factory.createSection(measpoints[0], mid12, measpoints[1]);

        sections.push_back(std::move(log1));

    }
    else {

        Point mid23;
        std::unique_ptr<Section> coarse;
        std::unique_ptr<Section> log;


        /* C++ Note: declare variables within the scope of the for-loop to allow the variable to be re-used on each iteration */

        for (int i = 0; i < numpts - 2; i++){

            /* Create mid-points and calculate mid-point radius using the shape of the coarser stem section. */

            coarse = section_factory.createSection(measpoints[i], measpoints[i + 1], measpoints[i + 2]);


            /* Midpoint between the first two measures is initially calculated from the coarse section, then averaged with the
            midpoint between the same measures on the previous coarse log. Only the section between the first two measures is
            stored. */

            log = section_factory.subdivideSection(coarse, coarse->first, coarse->second);

            #ifdef DEBUG

                Rcpp::Rcout << coarse->print() << std::endl;
                Rcpp::Rcout << "First midpoint: " << (log->second).print() << std::endl;

            #endif

            if(sections.begin() != sections.end()){
                log->second = average(log->second, mid23);
            }

            std::unique_ptr<Section> log23 = section_factory.subdivideSection(coarse, coarse->second, coarse->third);
            mid23 = log23->second;

            #ifdef DEBUG

                Rcpp::Rcout << "First midpoint (updated): " << (log->second).print() << std::endl;
                Rcpp::Rcout << "temp: " << mid23.print() << std::endl << std::endl;

            #endif


            if (log == nullptr){

                msg << "Tree::Tree: nullptr returned from createSection." << std::endl;
                msg << coarse->print();

                throw std::runtime_error(msg.str());
            }

            sections.push_back(std::move(log));

        }


        /* Final section, between final measure and the tree top */

        log = section_factory.createSection(coarse->second, mid23, coarse->third);
        sections.push_back(std::move(log));

    }

}




std::string Tree::print() const {

    std::ostringstream msg;

    msg << "Tree: " << std::endl;
    msg << stump->print();

    for(auto it = sections.begin(); it != sections.end(); it++){

        msg << (*it)->print() << std::endl;

    }

    return msg.str();

}






/* Height at which the tree has the given radius */
double Tree::height(double rad) const {			// formerly double htd(double d1);

	std::ostringstream msg;

    if (rad < 0.0){
		msg << "Tree::height: Supplied radius (" << rad << ") less than zero" << std::endl;
        throw std::invalid_argument(msg.str());
    }

    if (rad > ground_radius() || rad < (last_measure()).radius){
		return -HUGE_VAL;
    }


    // Find the section with first.radius < rad < third.radius
    auto it = std::find_if(sections.begin(), sections.end(),
		[rad](const std::unique_ptr<Section> &elem){return elem->contains_radius(rad); });


    if (it == sections.end()){

        if (stump->contains_radius(rad)){     // Check whether radius is between ground and the first measure

            return stump->height(rad);

        } else {  // If the radius couldn't be found in the tree, then throw a tantrum

            msg << "Tree::height: Could not find radius (" << rad << ") in tree: " << std::endl;
            msg << print() << std::endl;
            throw std::domain_error(msg.str());

        }

    }


    // Calculate the height corresponding to the radius in the given section
    double ht = (*it)->height(rad);


    // If the returned height is greater than the tree height, then pitch a fit.
    if (ht > treeht){
		msg << "Tree::height: Calculated height (" << ht << ") exceeds the tree height (" << treeht << ")" << std::endl;
        throw std::domain_error(msg.str());
    }


    // If the returned height is not in the section, then cry about it.
    if (!((*it)->contains_height(ht))){
		msg << "Tree::height: Calculated height (" << ht << ") does not lie in section: [" <<
        (*it)->first.hag << ", " << (*it)->third.hag << "]" << "  ht = " << ht << std::endl;
        throw std::domain_error(msg.str());
    }


    return ht;
}




/* Calculate the radius of the tree at the given height above ground */

double Tree::radius(double ht) const {			// formerly double dht(double h1);

	std::ostringstream msg;

    if (ht < 0.0){
		msg << "Tree::radius: Supplied height (" << ht << ") less than zero" << std::endl;
		throw std::invalid_argument(msg.str());
    }

    if (ht > treeht){
		return -HUGE_VAL;
    }


    // Check whether height is below the first measure; use neiloid calculation if so, otherwise interpolate between measures.
    double rad = 0.0;
    if (stump->contains_height(ht)){

        return stump->radius(ht);

    }
    else {

        // Find the section containing the given height
        auto jt = std::find_if(sections.begin(), sections.end(),
                          [ht](const std::unique_ptr<Section> &elem){return elem->contains_height(ht); });


        // If a section could not be found, then throw a wobbly.
        if (jt == sections.end()){

            msg << "Tree::radius: Could not find height (" << ht << ") in tree: " << std::endl;
            msg << this->print() << std::endl;
            throw std::domain_error(msg.str());

        }


        // Calculate the radius for the corresponding height in the given section
        rad = (*jt)->radius(ht);


        // If the returned radius is not in the section, then light the beacons and call for aid.
        if (!((*jt)->contains_radius(rad))){

            msg << "Tree::radius: Calculated radius (" << rad << ") does not lie in section: [" <<
            (*jt)->first.radius << ", " << (*jt)->third.radius << "]" << "  ht = " << ht << std::endl;
            throw std::domain_error(msg.str());

        }
    }


    return rad;
}




/* Calculate the volume between the ground and the given height along the stem */

double Tree::volume_to_height(double ht, bool abovestump) const {      /* previously double volh(double) */


    double vol = 0.0;


    if (ht < 0.0 || ht > (treeht)){
        return -HUGE_VAL;
    }


    if(abovestump && ht <= stump_height()){     // not using stump->contains_height(ht) because that returns true if ht <= (stump->first).height
        return 0.0;
    }


    // Volume of the entire tree, which there is already a function for.
    if (ht == (treeht)){

        vol = total_volume();

        if(abovestump){
            return vol - stump_vol();
        } else {
            return vol;
        }

    }


    if (stump->contains_height(ht)){

        if(abovestump){
            return stump->volume(stump_radius(), stump->radius(ht));
        } else {
            return stump->volume(ground_radius(), stump->radius(ht));
        }

    }


    // Volume between the stump and the first measure

    vol = stump->total_volume();

    if(abovestump){
        vol -= stump_vol();
    }


    if (ht == (first_measure()).hag){
        return vol;
    }



    /* Accumulate volume of all sections below the given height

     Volpak code used Newton's formula for log volumation, despite there being equations for
     volume of each type of stem section. Code for using hyperboloid/paraboloid/conical volume
     equations are provided when compiled with DEBUG active.
     */

    auto last = std::find_if(sections.begin(), sections.end(),
                             [ht](const std::unique_ptr<Section> &elem){return elem->contains_height(ht); });


    for (auto it = sections.begin(); it != last; it++){

        vol += (*it)->total_volume();

        if (ht == (*it)->third.hag){
            return vol;
        }

    }


    // Both methods use the same process for the final section.
    double rad = (*last)->radius(ht);
    double firstrad = ((*last)->first).radius;

    vol += (*last)->volume(firstrad, rad);

    return vol;
}





/* Calculate the volume between the stump and the supplied radius */

double Tree::volume_to_radius(double rad, bool abovestump) const {			// formerly double vold(double d1)


    double vol = 0.0;


    if (rad < 0.0 || rad > stump_radius()){
        return -HUGE_VAL;
    }


    // Volume to ground level
    if (abovestump && rad >= stump_radius()){           // not using stump->contains_radius(rad) because that returns true if rad >= (stump->first).radius
        return 0.0;
    }


    // Volume of the entire tree, which there is already a function for.
    if (rad == 0.0){

        vol = total_volume();

        if(abovestump){

            return vol - stump_vol();

        } else {

            return vol;

        }

    }


    // radii greater than the first measure have their own function for volume.
    if (stump->contains_radius(rad)){

        if(abovestump){

            return stump->volume(stump_radius(), rad);

        } else {

            return stump->volume(ground_radius(), rad);

        }

    }


    // Volume between the stump and the first measure
    vol = stump->total_volume();

    if(abovestump){

        vol -= stump_vol();

    }


    if (rad == (first_measure()).radius){
        return vol;
    }



    /* Accumulate volume of all sections below the given height

     Volpak code used Newton's formula for log volumation, despite there being equations for
     volume of each type of stem section. Code for using hyperboloid/paraboloid/conical volume
     equations are provided when compiled with DEBUG active.
     */

    auto last = std::find_if(sections.begin(), sections.end(),
                             [rad](const std::unique_ptr<Section> &elem){return elem->contains_radius(rad); });


    for (auto it = sections.begin(); it != last; it++){

        vol += (*it)->total_volume();

        if (rad == (*it)->third.radius){
            return vol;
        }
    }


    // Both methods use the same process for the final section.

    double firstrad = ((*last)->first).radius;
    vol += (*last)->volume(firstrad, rad);

    return vol;
}




double Tree::stump_radius() const {

    return (stump->stump).radius;

}



double Tree::stump_height() const {

    return STUMPHT;

}


double Tree::ground_radius() const {

    return (stump->ground).radius;

}




double Tree::stump_vol() const {

    return stump->stump_vol();

}




double Tree::vol_to_first_measure() const {

    return stump->total_volume();

}



double Tree::total_volume() const {

    double vol = vol_to_first_measure();

    for (auto it = sections.begin(); it != sections.end(); it++){

        vol += (*it)->total_volume();

    }

    return vol;

}




bool Tree::check_totht() const {

    /* cos(20.0) was the original volpak value: don't know if they really meant 20 radians, or if they intended 20 degrees.
     as cos(20.0) ~= 0.34 it seems pretty unlikely that this condition would trigger, unlike when using
     cos(20 * pi / 180) ~= 0.94, which aligns with the 6% required by the DST field manual.
     */
    bool response = true;
    if (treeht > 0.0){
        response = treeht < cos(20.0 * M_PI / 180) * (last_measure()).hag;
    }

    return response;
}





Point Tree::first_measure() const {

    return (sections[0])->first;

}



Point Tree::last_measure() const {

    return (sections.back())->third;

}
