
#include <stdlib.h>
#include <Rcpp.h>

#include "volpak_tree.h"




#define DEBUG
//#undef DEBUG








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


#ifdef DEBUG
    Rprintf("%s", tree.print());
#endif


	Tree* treeptr = &tree;

    Rcpp::S4 Rtree("volpak_tree");
	Rtree.slot("xptr") = Rcpp::XPtr<Tree>(treeptr, true);

    return Rtree;

}





// [[Rcpp::export]]
Rcpp::NumericVector r_total_vol(Rcpp::LogicalVector abovestump, Rcpp::S4 tree){

    if(!tree.hasSlot("xptr")){
        Rcpp::stop("Argument 'tree' does not have slot 'xptr'");
    }

    Rcpp::XPtr<Tree> xptr = tree.slot("xptr");


    if(abovestump.size() > 1){

      Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

    }


    double stumpvol = 0.0;

    if(abovestump[0]){

      stumpvol = xptr->stump_vol();

    }


    Rcpp::NumericVector vol(1);
    vol[0] = xptr->total_volume() - stumpvol;

    return vol;
}






// [[Rcpp::export]]
Rcpp::NumericVector r_get_hag(Rcpp::NumericVector search_radius, Rcpp::S4 tree){

    Rcpp::NumericVector hag(search_radius.size());
  Rcpp::XPtr<Tree> xptr = tree.slot("xptr");

    for (auto it = hag.begin(); it != hag.end(); it++){

		int i = std::distance(hag.begin(), it);

        *it = xptr->height(search_radius[i]);

    }

    return hag;

}






// [[Rcpp::export]]
Rcpp::NumericVector r_vol_to_tdub(Rcpp::NumericVector tdub, Rcpp::LogicalVector abovestump, Rcpp::S4 tree){

    Rcpp::NumericVector vol(tdub.size());
  Rcpp::XPtr<Tree> xptr = tree.slot("xptr");


  if(abovestump.size() > 1){

    Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

  }


  double stumpvol = 0.0;

  if(abovestump[0]){

    stumpvol = xptr->stump_vol();

  }


    for(int i = 0; i < vol.size(); i++){

        if (tdub[i] > 0){

            vol[i] = xptr->volume_to_radius(tdub[i], abovestump[0]);

        }
        else {

            vol[i] = xptr->total_volume() - stumpvol;

        }

    }

    return vol;

}





// [[Rcpp::export]]
Rcpp::NumericVector r_vol_to_hag(Rcpp::NumericVector hag, Rcpp::LogicalVector abovestump, Rcpp::S4 tree){

    Rcpp::NumericVector vol(hag.size());
  Rcpp::XPtr<Tree> xptr = tree.slot("xptr");


  if(abovestump.size() > 1){

    Rcpp::warning("Argument 'abovestump' contains more than one element, using first only.");

  }


  double stumpvol = 0.0;

  if(abovestump[0]){

    stumpvol = xptr->stump_vol();

  }


    for(int i = 0; i < vol.size(); i++){

        if(hag[i] >= xptr->treeht){

          vol[i] = xptr->total_volume() - stumpvol;

        } else {

          vol[i] = xptr->volume_to_height(hag[i], abovestump[0]) - stumpvol;

        }

    }

    return vol;
}











