## Total volume above the stump - formerly volpak::vtm
##



#' @export
#' @rdname volpak_tree
volpak_total_vol <- function(heights, diameters, total.height, stump.height = 0.15){

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

  idx <- order(heights, decreasing = FALSE)
  r_total_vol(heights[idx], radius[idx], total.height[1], stump.height[1])
}



