#' @export
#' @rdname volpak_tree
volpak_vol_to_tdub <- function(tree, tdubs, abovestump = FALSE){

  if(!inherits(tree, "volpak_tree")){
    stop("Argument 'tree' must inherit from 'volpak_tree'")
  }

  stump <- slot(slot(tree, "stump"), "points")
  stumprad <- stump$Diameter[stump$Label == "stump"]
  groundrad <- stump$Diameter[stump$Label == "ground"]

  if(abovestump && any(tdubs > stumprad)){
    warning("Requested 'tdubs' exceed stump radius, returning NA")
  } else if (any(tdubs > groundrad)){
    warning("Requested 'tdubs' exceed ground radius, returning NA")
  }

  abovestump <- as.logical(abovestump)

  vol <- r_vol_to_tdub(tree, tdubs/200, abovestump)

  if(any(is.infinite(vol))){
    vol <- ifelse(is.infinite(vol), NA, vol)
  }

  return(vol)

}




#' @export
#' @rdname volpak_tree
volpak_vol_to_hag <- function(tree, hags, abovestump = FALSE){

  if(!inherits(tree, "volpak_tree")){
    stop("Argument 'tree' must inherit from 'volpak_tree'")
  }

  if(all(is.na(hags))){
    return(NA)
  } else if (any(is.na(hags))){
    hags <- hags[!is.na(hags)]
  }

  if(any(hags > volpak_tree_height(tree))){
    warning("Requested 'hags' exceed tree height, returning NA")
  }

  abovestump <- as.logical(abovestump)

  vol <- r_vol_to_hag(tree, hags, abovestump)

  if(any(is.infinite(vol))){
    vol <- ifelse(is.infinite(vol), NA, vol)
  }

  return(vol)

}





#' @export
#' @rdname volpak_tree
volpak_total_vol <- function(tree, abovestump = FALSE){

  abovestump <- as.logical(abovestump)

  vol <- r_total_vol(tree, abovestump)

  if(is.infinite(vol)){
    vol <- NA
  }

  return(vol)

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







volpak_list_measures <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_list_measures(tree)

}






volpak_list_sections <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_list_sections(tree)

}





volpak_list_stump <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_list_stump(tree)

}



#' @export
#' @rdname volpak_tree
volpak_tree_height <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_volpak_tree_height(tree)

}



#' @export
#' @rdname volpak_tree
volpak_stump_radius <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_volpak_stump_radius(tree)

}




#' @export
#' @rdname volpak_tree
volpak_stump_vol <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_volpak_stump_vol(tree)

}




#' @export
#' @rdname volpak_tree
volpak_ground_radius <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_volpak_ground_radius(tree)

}




#' @export
#' @rdname volpak_tree
volpak_print <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  r_volpak_print(tree)

}





#' @export
#' @rdname volpak_tree
volpak_vol_above_top <- function(tree){

  if(!inherits(tree, "volpak_tree")){
    stop("`tree` must inherit from class 'volpak_tree'")
  }

  vol <- r_volpak_vol_above_top(tree)

  if(any(is.infinite(vol))){
    vol <- ifelse(is.infinite(vol), NA, vol)
  }

  return(vol)

}










