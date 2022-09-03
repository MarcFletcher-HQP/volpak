## S4 class - volpak_section
##




setClass(
  Class = "volpak_section",
  slots = list(
    points = "data.frame",
    a = "numeric",
    p = "numeric",
    q = "numeric"
  )
)





#' @rdname volpak_tree
#' @export
volpak_section <- function(base, mid = NULL, top, a = NULL, p = NULL, q = NULL){

  if(!inherits(base, "volpak_point") ||
     !inherits(top, "volpak_point")){
    stop("Inputs must inherit from S4 class 'volpak_point'")
  }

  if(!is.null(mid) && !inherits(mid, "volpak_point")){
    stop("Inputs must inherit from S4 class 'volpak_point'")
  }

  if(!(length(c(a, p, q)) %in% c(0, 3))) { # Either they're all NULL or they're all provided.
    stop("Provide all parameters, or none.")
  }


  if(is.null(a) && !is.null(mid)){
    r_volpak_section(base, mid, top)
  } else if (!is.null(a) && !is.null(mid)){
    r_volpak_section(base, mid, top, a, p, q)
  } else if (!is.null(a) && is.null(mid)) {
    r_volpak_section(base, top, a, p, q)
  } else {
    stop("Unhandled combination of inputs: is.null(a) && is.null(mid)")
  }
}




#' @export
#' @rdname volpak_tree
volpak_section_vol <- function(section){

  if(!inherits(section, "volpak_section")){
    stop("Argument 'section' must inherit from S4 class 'volpak_section'")
  }

  return(r_vol_section(section))
}
