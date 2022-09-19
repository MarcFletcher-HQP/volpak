

#' @useDynLib volpak, .registration = TRUE
#' @importFrom Rcpp sourceCpp
NULL




.onUnload <- function(libpath){
    library.dynam.unload("volpak", libpath)
}





