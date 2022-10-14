
#include <stdlib.h>
#include <Rcpp.h>

#include "volpak_tree.h"




#define DEBUG
#undef DEBUG




class TreeContainer{

public:

  std::shared_ptr<Tree> tree;

  TreeContainer(std::vector<double> height, std::vector<double> radius, double total_height, double stump_height) :
    tree(std::make_shared<Tree>(height, radius, total_height, stump_height)){}

};




// [[Rcpp::export]]
Rcpp::S4 r_volpak_tree(Rcpp::NumericVector height,
              Rcpp::NumericVector radius,
              Rcpp::NumericVector total_height,
              Rcpp::NumericVector stump_height){

  Rcpp::XPtr<TreeContainer> ptr(
    new TreeContainer(
      Rcpp::as<std::vector<double>>(radius),
      Rcpp::as<std::vector<double>>(height),
      total_height[0],
      stump_height[0]
    ),
    true
  );

#ifdef DEBUG
    Rcpp::Rcout << (ptr->tree)->print() << std::endl;
#endif

    Rcpp::S4 Rtree("volpak_tree");
    Rtree.slot("xptr") = ptr;

    return Rtree;

}





// [[Rcpp::export]]
Rcpp::NumericVector r_total_vol(Rcpp::S4 tree, Rcpp::LogicalVector abovestump){

  if(!tree.hasSlot("xptr")){
      Rcpp::stop("Argument 'tree' does not have slot 'xptr'");
  }

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");


  if(abovestump.size() > 1){

    Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

  }


  Rcpp::NumericVector vol(1);
  vol[0] = xptr->tree->total_volume() - xptr->tree->stump_vol();

  return vol;

}






// [[Rcpp::export]]
Rcpp::NumericVector r_get_hag(Rcpp::S4 tree, Rcpp::NumericVector search_radius){

  Rcpp::NumericVector hag(search_radius.size());
  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  for (auto it = hag.begin(); it != hag.end(); it++){

    int i = std::distance(hag.begin(), it);

    *it = xptr->tree->height(search_radius[i]);

  }

  return hag;

}





// [[Rcpp::export]]
Rcpp::NumericVector r_get_radius(Rcpp::S4 tree, Rcpp::NumericVector search_height){

  Rcpp::NumericVector rad(search_height.size());
  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  for (auto it = rad.begin(); it != rad.end(); it++){

    int i = std::distance(rad.begin(), it);

    *it = xptr->tree->radius(search_height[i]);

  }

  return rad;

}






// [[Rcpp::export]]
Rcpp::NumericVector r_vol_to_tdub(Rcpp::S4 tree, Rcpp::NumericVector tdub, Rcpp::LogicalVector abovestump){

  Rcpp::NumericVector vol(tdub.size());
  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");


  if(abovestump.size() > 1){

    Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

  }


  for(int i = 0; i < vol.size(); i++){

    vol[i] = xptr->tree->volume_to_radius(tdub[i], abovestump[0]);

  }

  return vol;

}





// [[Rcpp::export]]
Rcpp::NumericVector r_vol_to_hag(Rcpp::S4 tree, Rcpp::NumericVector hag, Rcpp::LogicalVector abovestump){

  Rcpp::NumericVector vol(hag.size());
  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");


  if(abovestump.size() > 1){

    Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

  }


  for(int i = 0; i < vol.size(); i++){

    vol[i] = xptr->tree->volume_to_height(hag[i], abovestump[0]);

  }


  return vol;
}



// [[Rcpp::export]]
Rcpp::DataFrame r_display_volpak_tree(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  int numsections = std::distance(xptr->tree->sections_begin(), xptr->tree->sections_end());
  int numpts = 2 * numsections + 3;    // first and second for each section, +1 for ground, +1 for stump, +1 for tree height

  Rcpp::NumericVector heights(numpts);
  Rcpp::NumericVector diams(numpts);
  Rcpp::CharacterVector labels(numpts);

  heights[0] = 0.0;
  heights[1] = xptr->tree->stump_height();

  diams[0] = xptr->tree->ground_radius() * 200;
  diams[1] = xptr->tree->stump_radius() * 200;

  labels[0] = "ground";
  labels[1] = "stump";


  for(auto it = xptr->tree->sections_begin(); it != xptr->tree->sections_end(); it++){

    int i = 2 * std::distance(xptr->tree->sections_begin(), it) + 2;

    heights[i] = ((*it)->first).hag;
    heights[i+1] = ((*it)->second).hag;

    diams[i] = ((*it)->first).radius * 200;
    diams[i+1] = ((*it)->second).radius * 200;

    labels[i] = "measure";
    labels[i+1] = "midpoint";

  }

  heights[numpts-1] = xptr->tree->get_treeht();
  diams[numpts-1] = 0.0;
  labels[numpts-1] = "measure";


  Rcpp::DataFrame df = Rcpp::DataFrame::create(
    Rcpp::Named("Height") = heights,
    Rcpp::Named("Diameter") = diams,
    Rcpp::Named("Label") = labels
  );


  df.attr("StumpType") = xptr->tree->stump_type();

  return df;

}

