## S4 Class definitions
##


setClass(
  Class = "volpak_stump",
  slots = list(
    points = "data.frame",
    type = "character"
  ),
  validity = function(object){

    if(!setequal(name(object@points), c("Height", "Diameter", "Label"))){

      stop("data.frame in slot 'points' does not contain the right header")

    }

    return(TRUE)

  }
)




setClass(
  Class = "volpak_section",
  slots = list(
    points = "data.frame",
    type = "character",
    p = "numeric",
    q = "numeric"
  ),
  validity = function(object){

    if(!setequal(name(object@points), c("Height", "Diameter", "Label"))){

      stop("data.frame in slot 'points' does not contain the right header")

    }

    return(TRUE)

  }
)




setClass(
  Class = "volpak_tree",
  slots = list(
    xptr = "externalptr",
    points = "data.frame",
    stump = "volpak_stump",
    sections = "list"
  ),
  validity = function(object){

    if(!setequal(name(object@points), c("Height", "Diameter", "Label"))){

      stop("data.frame in slot 'points' does not contain the right header")

    }


    if(!all(vapply(sections, inherits, logical(1), "volpak_section"))){

      stop("list in slot 'sections' can only contain elements that inherit from 'volpak_section'")

    }


    return(TRUE)

  }
)












