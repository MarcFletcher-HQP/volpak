
#include <stdlib.h>
#include <Rcpp.h>

#include "volpak_tree.h"




#define DEBUG
//#undef DEBUG




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


    double stumpvol = 0.0;

    if(abovestump[0]){

      stumpvol = xptr->tree->stump_vol();

    }


    Rcpp::NumericVector vol(1);
    vol[0] = xptr->tree->total_volume() - stumpvol;

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
Rcpp::NumericVector r_vol_to_tdub(Rcpp::S4 tree, Rcpp::NumericVector tdub, Rcpp::LogicalVector abovestump){

    Rcpp::NumericVector vol(tdub.size());
  Rcpp::XPtr<TreeContainer> xptr = tree.slot("xptr");


  if(abovestump.size() > 1){

    Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

  }


  double stumpvol = 0.0;

  if(abovestump[0]){

    stumpvol = xptr->tree->stump_vol();

  }


    for(int i = 0; i < vol.size(); i++){

        if (tdub[i] > 0){

            vol[i] = xptr->tree->volume_to_radius(tdub[i], abovestump[0]);

        }
        else {

            vol[i] = xptr->tree->total_volume() - stumpvol;

        }

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


  double stumpvol = 0.0;

  if(abovestump[0]){

    stumpvol = xptr->tree->stump_vol();

  }


    for(int i = 0; i < vol.size(); i++){

        if(hag[i] >= xptr->tree->treeht){

          vol[i] = xptr->tree->total_volume() - stumpvol;

        } else {

          vol[i] = xptr->tree->volume_to_height(hag[i], abovestump[0]) - stumpvol;

        }

    }

    return vol;
}











