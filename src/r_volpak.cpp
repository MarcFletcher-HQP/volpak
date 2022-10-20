
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
Rcpp::NumericVector r_total_vol(Rcpp::S4 tree, Rcpp::LogicalVector abovestump, Rcpp::LogicalVector abovetop){

  if(!tree.hasSlot("xptr")){
      Rcpp::stop("Argument 'tree' does not have slot 'xptr'");
  }

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");


  if(abovestump.size() > 1){

    Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

  }

  if(abovetop.size() > 1){

    Rcpp::warning("Argument 'abovetop' contains more than one element, using first only.");

  }


  Rcpp::NumericVector vol(1);
  vol[0] = xptr->tree->total_volume();

  if(abovestump[0]){
    vol[0] -= xptr->tree->stump_vol();
  }

  if(abovetop[0]){
    vol[0] += xptr->tree->vol_above_top();
  }

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
Rcpp::DataFrame r_list_measures(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  bool treehtused = xptr->tree->get_treeht() > 0.0;
  int numsections = std::distance(xptr->tree->sections_begin(), xptr->tree->sections_end());
  int numpts = 2 * numsections + ((int) treehtused) + 2;    // first and second for each section, treeht (if used), +1 for ground, and +1 for stump.

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


  if(treehtused){

    double treeht = xptr->tree->get_treeht();

    heights[numpts-1] = treeht;
    diams[numpts-1] = 0.0;
    labels[numpts-1] = "measure";

  } else {

    auto it = xptr->tree->sections_end() - 1;

    heights[numpts-1] = ((*it)->third).hag;
    diams[numpts-1] = ((*it)->third).radius * 200;
    labels[numpts-1] = "measure";

  }


  Rcpp::DataFrame df = Rcpp::DataFrame::create(
    Rcpp::Named("Height") = heights,
    Rcpp::Named("Diameter") = diams,
    Rcpp::Named("Label") = labels
  );


  df.attr("StumpType") = xptr->tree->stump_type();

  return df;

}






// [[Rcpp::export]]
Rcpp::List r_list_sections(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  int numsections = std::distance(xptr->tree->sections_begin(), xptr->tree->sections_end());

  Rcpp::List _sections(numsections);

  SectionFactory factory;


  for(auto it = xptr->tree->sections_begin(); it != xptr->tree->sections_end(); it++){

    Rcpp::S4 thisection("volpak_section");

    Rcpp::NumericVector heights(3), diams(3);
    Rcpp::CharacterVector labels(3);

    heights[0] = ((*it)->first).hag;
    heights[1] = ((*it)->second).hag;
    heights[2] = ((*it)->third).hag;

    diams[0] = ((*it)->first).radius * 200;
    diams[1] = ((*it)->second).radius * 200;
    diams[2] = ((*it)->third).radius * 200;

    labels[0] = "measure";
    labels[1] = "midpoint";
    labels[2] = "measure";

    Rcpp::DataFrame df = Rcpp::DataFrame::create(
      Rcpp::Named("Height") = heights,
      Rcpp::Named("Diameter") = diams,
      Rcpp::Named("Label") = labels
    );

    thisection.slot("points") = df;


    SectionFactory::SectionType type = factory.getSectionType(const_cast<std::unique_ptr<Section>&>(*it));
    thisection.slot("type") = factory.printSectionType(type);


    thisection.slot("p") = (*it)->p;
    thisection.slot("q") = (*it)->q;


    int i = std::distance(xptr->tree->sections_begin(), it);

    _sections[i] = thisection;

  }


  return _sections;

}




// [[Rcpp::export]]
Rcpp::S4 r_list_stump(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");


  Rcpp::NumericVector heights(2), diams(2);
  Rcpp::CharacterVector labels(2);


  heights[0] = 0.0;
  heights[1] = xptr->tree->stump_height();

  diams[0] = xptr->tree->ground_radius() * 200;
  diams[1] = xptr->tree->stump_radius() * 200;

  labels[0] = "ground";
  labels[1] = "stump";

  Rcpp::DataFrame df = Rcpp::DataFrame::create(
    Rcpp::Named("Height") = heights,
    Rcpp::Named("Diameter") = diams,
    Rcpp::Named("Label") = labels
  );



  Rcpp::S4 stump("volpak_stump");

  stump.slot("points") = df;
  stump.slot("type") = xptr->tree->stump_type();



  return stump;

}



// [[Rcpp::export]]
Rcpp::NumericVector r_volpak_tree_height(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  Rcpp::NumericVector tht(1);

  tht[0] = xptr->tree->tree_height();

  return tht;

}




// [[Rcpp::export]]
Rcpp::NumericVector r_volpak_stump_radius(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  Rcpp::NumericVector stumprad(1);

  stumprad[0] = xptr->tree->stump_radius();

  return stumprad;

}



// [[Rcpp::export]]
Rcpp::NumericVector r_volpak_stump_vol(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  Rcpp::NumericVector stumpvol(1);

  stumpvol[0] = xptr->tree->stump_vol();

  return stumpvol;

}



// [[Rcpp::export]]
Rcpp::NumericVector r_volpak_ground_radius(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  Rcpp::NumericVector groundrad(1);

  groundrad[0] = xptr->tree->ground_radius();

  return groundrad;

}



// [[Rcpp::export]]
void r_volpak_print(Rcpp::S4 tree){

  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");

  Rcpp::Rcout << xptr->tree->print() << std::endl;

  return;

}