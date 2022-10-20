## volpak - C interface. Used for testing.
##




#' @export
volpak_vpakinit <- function(heights, diameters, total.height){

  total.height <- unique(total.height)


  if(any(is.na(heights) | is.na(diameters) | is.na(total.height))){
    stop("NA values found in input")
  }

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


  if(any(is.na(heights) | is.na(diameters) | is.na(total.height))){
    stop("NA values found in input")
  }

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
  vol <- r_vtm(heights[idx], diameters[idx], total.height[1])

  if(vol < 0){
    vol <- 0
  }

  return(vol)
}




#' @export
volpak_vold <- function(vol.to.diam, heights, diameters, total.height){

  total.height <- unique(total.height)


  if(any(is.na(heights) | is.na(diameters) | is.na(total.height))){
    stop("NA values found in input")
  }

  if(all(is.na(vol.to.diam))){

    return(NA)

  } else if (any(is.na(vol.to.diam))){

    vol.to.diam <- vol.to.diam[!is.na(vol.to.diam)]

  }

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
  vol <- r_vold(vol.to.diam, heights[idx], diameters[idx], total.height[1])

  if(any(vol < 0)){
    vol[vol < 0] <- 0
  }

  return(vol)
}




#' @export
volpak_volh <- function(vol.to.hag, heights, diameters, total.height){

  total.height <- unique(total.height)


  if(any(is.na(heights) | is.na(diameters) | is.na(total.height))){
    stop("NA values found in input")
  }

  if(all(is.na(vol.to.hag))){

    return(NA)

  } else if (any(is.na(vol.to.hag))){

    vol.to.hag <- vol.to.hag[!is.na(vol.to.hag)]

  }

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
  vol <- r_volh(vol.to.hag, heights[idx], diameters[idx], total.height[1])

  if(any(vol < 0)){
    vol[vol < 0] <- 0
  }

  return(vol)

}




#' @export
volpak_htd <- function(search.diameters, heights, diameters, total.height){

  total.height <- unique(total.height)


  if(any(is.na(heights) | is.na(diameters) | is.na(total.height))){
    stop("NA values found in input")
  }

  if(all(is.na(search.diameters))){

    return(NA)

  } else if (any(is.na(search.diameters))){

    search.diameters <- search.diameters[!is.na(search.diameters)]

  }

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
  vol <- r_htd(search.diameters, heights[idx], diameters[idx], total.height[1])

  if(any(vol < 0)){
    vol[vol < 0] <- 0
  }

  return(vol)

}





