##
##



#' R Interface to Volpak routines
#'
#' \code{volpak_tree} returns an object of type \linkS4class{volpak_tree}
#' containing the information stored in the \code{Tree} class, used by volpak.
#' \code{volpak_total_vol} calculates the total under-bark volume between a 15cm
#' stump and the top of the tree. \code{volpak_get_hag} calculates the height
#' above ground at which the tree has the requested diameter.
#' \code{volpak_vol_to_tdub} calculates the volume between a 15cm stump and the
#' under-bark diameter provided. \code{volpak_vol_to_hag} calculates the volume
#' between a 15cm stump and the height above ground provided.
#'
#' The volpak library is used by GAME to produce volume estimates, as well as
#' interpolated heights and diameters, for trees that have been measured as park
#' of HQPlantations DST program.
#'
#' The interface between R and volpak has been written for simplicity, as such
#' all inputs must be relevant to a single tree. An interface to the volpak
#' initialisation routine has been provided, however each of the exported
#' functions in the package will also initialise volpack internally.
#'
#'
#' @param heights The height above ground corresponding to each diameter
#'   measurement.
#' @param diameters The under-bark diameter measured along the tree.
#' @param total.height The total height of the tree.
#' @param stump.height The height (in metres above ground) of the stump.
#' @param abovestump Should calculated volumes exclude the 15cm stump.
#'
#' @return \code{volpak_tree} returns NULL, \code{volpak_total_vol} returns a
#'   single numeric value, all remaining functions return a numeric vector with
#'   the same length as the input vectors.
#' @export
#'
#' @examples
#' ht <- c(0.5, 1.3, 5, 8, 11, 14, 17, 20)
#' diam <- round(10/sqrt(ht), 1)
#' tht <- 22
#'
#' volpak_tree(ht, diam, tht)
#' volpak_total_vol(ht, diam, tht)
#' volpak_get_hag(c(5, 2.8), ht, diam, tht)
#' volpak_vol_to_tdub(c(5, 2.8), ht, diam, tht)
#' volpak_vol_to_hag(c(10, 13), ht, diam, tht)
volpak_tree <- function(heights, diameters, total.height, stump.height = 0.15){

  radius <- diameters / 200
  total.height <- unique(total.height)

  if(total.height[1] > 0 && any(heights > total.height[1])){
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
  tree <- r_volpak_tree(heights[idx], radius[idx], total.height[1], stump.height[1])


  tree@points <- volpak_list_measures(tree)
  tree@stump  <- volpak_list_stump(tree)
  tree@sections <- volpak_list_sections(tree)


  return(tree)

}






