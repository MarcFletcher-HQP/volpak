## Height (above ground) corresponding to the given diameter - formerly volpak::htd
##



#' @export
#' @rdname volpak_tree
volpak_get_hag <- function(search.diam, heights, diameters, total.height, stump.height = 0.15){

  radius <- diameters / 200
  search.radii <- search.diam / 200
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

  hags <- if(length(search.radii) > 0){
    idx <- order(heights, decreasing = FALSE)
    r_get_hag(search.radii, heights[idx], radius[idx], total.height[1], stump.height[1])
  } else {
    warning("search.diam is length zero")
    NULL
  }

  if(any(hags < 0)){
    hags[hags < 0] <- NA_real_
  }

  return(hags)
}
