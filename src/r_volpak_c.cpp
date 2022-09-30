

#include <stdlib.h>
#include <Rcpp.h>
#include "volpak_c.h"




// [[Rcpp::export]]
void r_vpakinit(Rcpp::NumericVector height,
                Rcpp::NumericVector diameter,
                Rcpp::NumericVector total_height){

    // Populate the arrays

    int N = height.size();
    double ht[NOMEAS];
    double d[NOMEAS];

    for(int i = 0; i < N; i++){
        ht[i] = height[i];
        d[i] = diameter[i];
    }

    double tht = total_height[0];

    ht[N] = tht;
    d[N] = 0.0;

    vpakinit(ht, d, tht, N+1);

    return;
}






// [[Rcpp::export]]
Rcpp::NumericVector r_vtm(Rcpp::NumericVector height,
                          Rcpp::NumericVector diameter,
                          Rcpp::NumericVector total_height){

    // Populate the arrays

    int N = height.size();
    double ht[NOMEAS];
    double d[NOMEAS];

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
Rcpp::NumericVector r_htd(Rcpp::NumericVector diams,
                          Rcpp::NumericVector height,
                          Rcpp::NumericVector diameter,
                          Rcpp::NumericVector total_height){

    // Populate the arrays

    int N = height.size();
    double ht[NOMEAS];
    double d[NOMEAS];

    for(int i = 0; i < N; i++){
        ht[i] = height[i];
        d[i] = diameter[i];
    }

    double tht = total_height[0];

    ht[N] = tht;
    d[N] = 0.0;

    vpakinit(ht, d, tht, N+1);

    Rcpp::NumericVector hag(diams.size());

    for(int i = 0; i < diams.size(); i++){

        hag[i] = htd(diams[i]);

    }

    return hag;

}





// [[Rcpp::export]]
Rcpp::NumericVector r_vold(Rcpp::NumericVector diams,
                           Rcpp::NumericVector height,
                           Rcpp::NumericVector diameter,
                           Rcpp::NumericVector total_height){

    // Populate the arrays

    int N = height.size();
    double ht[NOMEAS];
    double d[NOMEAS];

    for(int i = 0; i < N; i++){
        ht[i] = height[i];
        d[i] = diameter[i];
    }

    double tht = total_height[0];

    ht[N] = tht;
    d[N] = 0.0;

    vpakinit(ht, d, tht, N+1);

    Rcpp::NumericVector volume(diams.size());

    for(int i = 0; i < diams.size(); i++){

        volume[i] = vold(diams[i]);

    }


    return volume;

}




// [[Rcpp::export]]
Rcpp::NumericVector r_volh(Rcpp::NumericVector hags,
                           Rcpp::NumericVector height,
                           Rcpp::NumericVector diameter,
                           Rcpp::NumericVector total_height){

    // Populate the arrays

    int N = height.size();
    double ht[NOMEAS];
    double d[NOMEAS];

    for(int i = 0; i < N; i++){
        ht[i] = height[i];
        d[i] = diameter[i];
    }

    double tht = total_height[0];

    ht[N] = tht;
    d[N] = 0.0;

    vpakinit(ht, d, tht, N+1);

    Rcpp::NumericVector volume(hags.size());

    for(int i = 0; i < hags.size(); i++){

        volume[i] = volh(hags[i]);

    }


    return volume;

}

