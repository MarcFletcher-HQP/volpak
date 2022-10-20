## volpak - C interface. Used for testing.
##




#' @export
volpak_vpakinit <- function(heights, diameters, total.height){

  total.height <- unique(total.height)

  if(total.height > 0 && any(heights > total.height[1])){
    warning("heights in excess of total tree height in data")
  }

  if(length(heights) != length(diameters)){
    stop("length mismatch between heights and diameters")
  }

  if(length(total.height) > 1){
    stop("total.height must be unique")
  }

  idx <- order(heights, decreasing = FALSE)
  r_vpakinit(heights[idx], diameters[idx], total.height[1])
}




#' @export
volpak_vtm <- function(heights, diameters, total.height){

  total.height <- unique(total.height)

  if(total.height > 0 && any(heights > total.height[1])){
    warning("heights in excess of total tree height in data")
  }

  if(length(heights) != length(diameters)){
    stop("length mismatch between heights and diameters")
  }

  if(length(total.height) > 1){
    stop("total.height must be unique")
  }

  idx <- order(heights, decreasing = FALSE)
  r_vtm(heights[idx], diameters[idx], total.height[1])
}




#' @export
volpak_vold <- function(vol.to.diam, heights, diameters, total.height){

  total.height <- unique(total.height)

  if(total.height > 0 && any(heights > total.height[1])){
    warning("heights in excess of total tree height in data")
  }

  if(length(heights) != length(diameters)){
    stop("length mismatch between heights and diameters")
  }

  if(length(total.height) > 1){
    stop("total.height must be unique")
  }

  idx <- order(heights, decreasing = FALSE)
  r_vold(vol.to.diam, heights[idx], diameters[idx], total.height[1])
}




#' @export
volpak_volh <- function(vol.to.hag, heights, diameters, total.height){

  total.height <- unique(total.height)

  if(total.height > 0 && any(heights > total.height[1])){
    warning("heights in excess of total tree height in data")
  }

  if(length(heights) != length(diameters)){
    stop("length mismatch between heights and diameters")
  }

  if(length(total.height) > 1){
    stop("total.height must be unique")
  }

  idx <- order(heights, decreasing = FALSE)
  r_volh(vol.to.hag, heights[idx], diameters[idx], total.height[1])
}




#' @export
volpak_htd <- function(search.diameters, heights, diameters, total.height){

  total.height <- unique(total.height)

  if(total.height > 0 && any(heights > total.height[1])){
    warning("heights in excess of total tree height in data")
  }

  if(length(heights) != length(diameters)){
    stop("length mismatch between heights and diameters")
  }

  if(length(total.height) > 1){
    stop("total.height must be unique")
  }

  idx <- order(heights, decreasing = FALSE)
  r_htd(search.diameters, heights[idx], diameters[idx], total.height[1])
}


