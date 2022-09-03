## S4 class - volpak_point
##


setClass(
  Class = "volpak_point",
  slots = list(
    hag = "numeric",
    radius = "numeric"
  )
)






#' @rdname volpak_tree
#' @export
volpak_point <- function(hag, diam){

  r_volpak_point(hag, diam/200)

}
