#' @export
#' @rdname volpak_tree
volpak_vol_to_tdub <- function(tree, tdubs, abovestump = FALSE){

  if(!inherits(tree, "volpak_tree")){
    stop("Argument 'tree' must inherit from 'volpak_tree'")
  }

  abovestump <- as.logical(abovestump)

  r_vol_to_tdub(tree, tdubs/200, abovestump)

}




#' @export
#' @rdname volpak_tree
volpak_vol_to_hag <- function(tree, hags, abovestump = FALSE){

  if(!inherits(tree, "volpak_tree")){
    stop("Argument 'tree' must inherit from 'volpak_tree'")
  }

  abovestump <- as.logical(abovestump)

  r_vol_to_hag(tree, hags, abovestump)

}





#' @export
#' @rdname volpak_tree
volpak_total_vol <- function(tree, abovestump = FALSE){

  abovestump <- as.logical(abovestump)

  r_total_vol(tree, abovestump)

}





#' @export
#' @rdname volpak_tree
volpak_get_hag <- function(tree, search.diam){

  search.radii <- search.diam / 200

  hags <- r_get_hag(tree, search.radii)

  if(any(hags < 0)){
    hags[hags < 0] <- NA_real_
  }

  return(hags)
}




#' @export
#' @rdname volpak_tree
volpak_get_diam <- function(tree, search.hag){

  rads <- r_get_radius(tree, search.hag)

  if(any(rads < 0)){
    rads[rads < 0] <- NA_real_
  }

  diams <- rads * 200

  return(diams)
}






#' @export
#' @rdname volpak_tree
volpak_display_tree <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_volpak_display_tree(tree)

}



