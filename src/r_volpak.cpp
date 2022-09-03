#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Rcpp.h>
#include "GAMERoutines.h"
#include "volpak.h"

extern "C" {
#include "protoutility.h"	/* GAME libutility prototypes */
}




// [[Rcpp::export]]
Rcpp::S4 r_volpak_tree(Rcpp::NumericVector height, 
                       Rcpp::NumericVector radius, 
                       Rcpp::NumericVector total_height, 
                       Rcpp::NumericVector stump_height){
    
    // Initialise the tree
    Tree tree(
            Rcpp::as<std::vector<double>>(radius), 
            Rcpp::as<std::vector<double>>(height), 
            total_height[0], 
            stump_height[0]
    );
    
    
    // Create data.frame of measurements; including ground, top, and midpoints.
    Rcpp::NumericVector hags(tree.measpoints.size());
    Rcpp::NumericVector radii(tree.measpoints.size());
	Rcpp::CharacterVector type(tree.measpoints.size(), "Measure");
    Rcpp::LogicalVector neiloid_stump(1);

    neiloid_stump[0] = tree.neiloid_stump;

    
    for (auto it = tree.measpoints.begin(); it != tree.measpoints.end(); it++){
        int i = std::distance(tree.measpoints.begin(), it);
        hags[i] = tree.measpoints[i].hag;
        radii[i] = tree.measpoints[i].radius;
    }

	
	hags.insert(hags.begin(), tree.stump.hag);
	hags.insert(hags.begin(), tree.ground.hag);
	
	radii.insert(radii.begin(), tree.stump.radius);
	radii.insert(radii.begin(), tree.ground.radius);

	type.insert(type.begin(), "Stump");
	type.insert(type.begin(), "Ground");
    
    
    Rcpp::DataFrame points = Rcpp::DataFrame::create(
        Rcpp::Named("HAG") = clone(hags),
        Rcpp::Named("Radius") = clone(radii), 
		Rcpp::Named("Measure") = clone(type)
    );
    
    
    // Convert each section to 'volpak_section'
    Rcpp::NumericVector s_hags(3);
    Rcpp::NumericVector s_radii(3);
    Rcpp::DataFrame s_points;
    Rcpp::List sectionList;
    Rcpp::S4 section("volpak_section");
    
    for (auto it = tree.sections.begin(); it != tree.sections.end(); it++){
        
        s_hags[0] = it->first.hag;
        s_hags[1] = it->second.hag;
        s_hags[2] = it->third.hag;
        s_radii[0] = it->first.radius;
        s_radii[1] = it->second.radius;
        s_radii[2] = it->third.radius;
        
        s_points = Rcpp::DataFrame::create(
            Rcpp::Named("HAG") = clone(s_hags),
            Rcpp::Named("Radius") = clone(s_radii)
        );
        
        section.slot("points") = s_points;
        section.slot("a") = it->a;
        section.slot("p") = it->p;
        section.slot("q") = it->q;
        
        sectionList.push_back(clone(section));
    }


    Rcpp::S4 base("volpak_section");
    s_hags[0] = tree.base.first.hag;
    s_hags[1] = tree.base.second.hag;
    s_hags[2] = tree.base.third.hag;
    s_radii[0] = tree.base.first.radius;
    s_radii[1] = tree.base.second.radius;
    s_radii[2] = tree.base.third.radius;

    s_points = Rcpp::DataFrame::create(
        Rcpp::Named("HAG") = clone(s_hags),
        Rcpp::Named("Radius") = clone(s_radii)
    );

    base.slot("points") = s_points;
    base.slot("a") = tree.base.a;
    base.slot("p") = tree.base.p;
    base.slot("q") = tree.base.q;
    
    
    
    // Create volpak_tree S4 class and populate the fields appropriately
    Rcpp::S4 Rtree("volpak_tree");
    Rtree.slot("points") = points;
    Rtree.slot("sections") = sectionList;
    Rtree.slot("neiloid_stump") = neiloid_stump;
    Rtree.slot("base") = base;

	Tree* treeptr = &tree;
	Rtree.slot("xptr") = Rcpp::XPtr<Tree>(treeptr, true);
    
    return Rtree;
}





// [[Rcpp::export]]
Rcpp::S4 r_volpak_section(Rcpp::S4 base, Rcpp::S4 mid, Rcpp::S4 top){

	// Initialise the section
	if (!Rf_inherits(base, "volpak_point") || !Rf_inherits(mid, "volpak_point") || 
		!Rf_inherits(top, "volpak_point")){
		throw std::invalid_argument("inputs must inherit from class 'volpak_point'");
	}


	Rcpp::NumericVector hag1 = base.slot("hag"), rad1 = base.slot("radius");
	Rcpp::NumericVector hag2 = mid.slot("hag"),  rad2 = mid.slot("radius");
	Rcpp::NumericVector hag3 = top.slot("hag"),  rad3 = top.slot("radius");


	Point p_base(hag1[0], rad1[0]);
	Point p_mid(hag2[0], rad2[0]);
	Point p_top(hag3[0], rad3[0]);

	Section sect(p_base, p_mid, p_top);


	// Convert each section to 'volpak_section'
	Rcpp::NumericVector s_hags(3);
	Rcpp::NumericVector s_radii(3);
	Rcpp::DataFrame points;
	Rcpp::S4 section("volpak_section");

	s_hags[0] = sect.first.hag;
	s_hags[1] = sect.second.hag;
	s_hags[2] = sect.third.hag;
	s_radii[0] = sect.first.radius;
	s_radii[1] = sect.second.radius;
	s_radii[2] = sect.third.radius;

	points = Rcpp::DataFrame::create(
		Rcpp::Named("HAG") = s_hags,
		Rcpp::Named("Radius") = s_radii
	);

	section.slot("points") = points;
	section.slot("a") = sect.a;
	section.slot("p") = sect.p;
	section.slot("q") = sect.q;


	return section;
}



// [[Rcpp::export]]
Rcpp::List r_volpak_point(Rcpp::NumericVector hag, Rcpp::NumericVector radius){

    if(hag.size() != radius.size()){
        Rcpp::stop("Size mismatch of input arguments\n");
    }

    Rcpp::List pointList(hag.size());

    for(int i = 0; i < pointList.size(); i++){

        Rcpp::S4 point("volpak_point");
        point.slot("hag") = hag[i];
        point.slot("radius") = radius[i];

        pointList[i] = point;

    }

    return pointList;

}




// [[Rcpp::export]]
Rcpp::NumericVector r_vol_section(Rcpp::S4 log){
    
    // Get points associated with section.

    Rcpp::DataFrame points = log.slot("points");
    Rcpp::NumericVector hags = points["HAG"];
    Rcpp::NumericVector radii = points["Radius"];

    Point p_base(hags[0], radii[0]);
    Point p_mid(hags[1], radii[1]);
    Point p_top(hags[2], radii[2]);



    // Initialise the section
    Section section(p_base, p_mid, p_top);
    
    
    Rcpp::NumericVector vol(1);
    vol[0] = section.total_volume();
    
    
    return vol;
}







// [[Rcpp::export]]
Rcpp::NumericVector r_total_vol(Rcpp::NumericVector height, 
                                Rcpp::NumericVector radius, 
                                Rcpp::NumericVector total_height,
                                Rcpp::NumericVector stump_height){
    
    // Initialise the tree
    Tree tree(Rcpp::as<std::vector<double>>(radius), Rcpp::as<std::vector<double>>(height), total_height[0], stump_height[0]);
    
    
    Rcpp::NumericVector vol(1);
    vol[0] = tree.vol_above_stump();
    
    
    return vol;
}




// [[Rcpp::export]]
Rcpp::NumericVector r_vtm(Rcpp::NumericVector height, 
                          Rcpp::NumericVector diameter, 
                          Rcpp::NumericVector total_height){

    // Populate the arrays

    int N = height.size();
    double ht[50];
    double d[50];

    for(int i = 0; i < N; i++){
        ht[i] = height[i];
        d[i] = diameter[i];
    }

    double tht = total_height[0];

    ht[N] = tht;
    d[N] = 0.0;

    vpakinit(ht, d, tht, N+1);

    double vol = vtm();

    Rcpp::NumericVector total_volume(1);
    total_volume[0] = vol;

    return total_volume;
                            
}






// [[Rcpp::export]]
Rcpp::NumericVector r_get_hag(Rcpp::NumericVector search_radius, 
                              Rcpp::NumericVector height, 
                              Rcpp::NumericVector radius, 
                              Rcpp::NumericVector total_height, 
                              Rcpp::NumericVector stump_height){
    
    // Initialise the tree
    Tree tree(Rcpp::as<std::vector<double>>(radius), Rcpp::as<std::vector<double>>(height), total_height[0], stump_height[0]);
    
    
    // Calculate the height above ground for each radius supplied.
    Rcpp::NumericVector hag(search_radius.size());
    for (auto it = hag.begin(); it != hag.end(); it++){
        
		int i = std::distance(hag.begin(), it); 
        *it = tree.height_when(search_radius[i]);
        
    }
    
    return hag;
    
}






// [[Rcpp::export]]
Rcpp::NumericVector r_vol_to_tdub(Rcpp::NumericVector tdub, 
                                  Rcpp::NumericVector height, 
                                  Rcpp::NumericVector radius, 
                                  Rcpp::NumericVector total_height,
                                  Rcpp::NumericVector stump_height){
    
    // Initialise the tree
    Tree tree(Rcpp::as<std::vector<double>>(radius), Rcpp::as<std::vector<double>>(height), total_height[0], stump_height[0]);
    
    
    // Calculate the volume from a 15cm stump to the under-bark radius provided.
    Rcpp::NumericVector vol(tdub.size());
    for(int i = 0; i < vol.size(); i++){
        
        if (tdub[i] > 0){
            vol[i] = tree.volume_to_radius(tdub[i]);
        }
        else {
            vol[i] = tree.vol_above_stump();
        }
        
    }
    
    return vol;
}





// [[Rcpp::export]]
Rcpp::NumericVector r_vol_to_hag(Rcpp::NumericVector hag, 
                                 Rcpp::NumericVector height, 
                                 Rcpp::NumericVector radius, 
                                 Rcpp::NumericVector total_height,
                                 Rcpp::NumericVector stump_height){
    
    // Initialise the tree
    Tree tree(Rcpp::as<std::vector<double>>(radius), Rcpp::as<std::vector<double>>(height), total_height[0], stump_height[0]);
   
    
    // Calculate the volume from a 15cm stump to the under-bark radius provided.
    Rcpp::NumericVector vol(hag.size());
    for(int i = 0; i < vol.size(); i++){
        vol[i] = tree.volume_to_hag(hag[i]);
    }
    
    return vol;
}






// [[Rcpp::export]]
Rcpp::NumericVector r_treeht_est(
        Rcpp::IntegerVector method,
        Rcpp::NumericVector height,
        Rcpp::NumericVector radius,
        Rcpp::NumericVector total_height,
        Rcpp::NumericVector stump_height){
    
    Rcpp::NumericVector treeht(1);
    
    // Check inputs
    if (method[0] > 2){
        Rcpp::stop("Invalid method: %i", method);
    }
    
    if (height.length() < 3){
        Rcpp::stop("Insufficient number of measures: %i", height.length());
    }
    
    
    // Initialise the tree
    Tree tree(Rcpp::as<std::vector<double>>(radius), Rcpp::as<std::vector<double>>(height), total_height[0], stump_height[0]);
    
    
    // Estimate the tree height
    treeht[0] = tree.missing_height(method[0]);
    
    
    return treeht;
}










