## Volume, above stump, to the specified diameter - formerly volpak::vold
##




#' @export
#' @rdname volpak_tree
volpak_vol_to_tdub <- function(tdubs, heights, diameters, total.height, stump.height = 0.15){

  trubs <- tdubs / 200
  radius <- diameters / 200
  total.height <- unique(total.height)

  if(any(heights > total.height[1])){
    warning("heights in excess of total tree height in data")
  }

  if(length(heights) != length(diameters)){
    stop("length mismatch between heights and diameters")
  }

  if(length(total.height) > 1){
    stop("total.height must be unique")
  }

  if(length(stump.height) > 1){
    stop("stump.height must be unique")
  }

  hags <- if(length(trubs) > 0){
    idx <- order(heights, decreasing = FALSE)
    r_vol_to_tdub(trubs, heights[idx], radius[idx], total.height[1], stump.height[1])
  } else {
    warning("tdubs is length zero")
    NULL
  }

  return(hags)
}
