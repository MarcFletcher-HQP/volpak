

#####################
# Utility Functions #
#####################



factors_as_strings <- function(df){
    if(!is.list(df)){
        stop("Expected a list or data.frame, got: ", class(df))
    }
    
    df[] <- lapply(df, function(x) if(is.factor(x)) as.character(x) else x)
    df
}


mfloor <- function(x, base = 1){
    floor(x / base) * base
}

mround <- function(x, base = 1){
    round(x / base) * base
}

mceiling <- function(x, base = 1){
    ceiling(x / base) * base
}


.onUnload <- function(libpath){
    library.dynam.unload("GAMERoutines", libpath)
}




################
# Input Checks #
################


check_missing_arguments <- function(...){
    mc <- list(...)
    null.value <- vapply(mc, is.null, logical(1), USE.NAMES = FALSE)
    fail.check <- any(null.value)
    if(fail.check){
        args.bad <- names(mc)[which(null.value)]
        msg.bad  <- paste(args.bad, collapse = ", ") %>% 
            gsub(",(?!.*,)", " &", ., perl = TRUE)
        stop("The following inputs were missing: ", msg.bad, "\n")
    }
    
    return(fail.check)
}


length_one_input <- function(...){
    mc <- list(...)
    if(is.null(names(mc))){
        stop("arguments must be named")
    }
    
    mc <- lapply(mc, unique) %>% 
        setNames(names(mc))
    
    nLen <- vapply(mc, length, numeric(1))
    if(any(nLen > 1)){
        msg <- which(nLen > 1) %>% 
            extract(names(mc), .) %>% 
            paste(collapse = ", ") %>% 
            gsub(",(?!.*,)", " &", ., perl = TRUE)
        
        stop("Arguments with more than one unique value: ", msg)
    }
    
    return(mc)
}





#####################
# Derived Variables #
#####################


#' Derive Planting Rotation from Compartment Name
#'
#' \code{get_rotation} examines the compartment name to determine the planting
#' rotation, using additional information on financial year of planting and
#' logging area to manage an edge case.
#'
#' Compartment designations have evolved over time to include information on the
#' planted rotation. Compartments numbered less than 200 are a first rotation
#' planting, those numbered 200-299 are a second rotation planting and some 300+
#' series compartments (being a third rotation) are begining to be planted in
#' some areas. Occasionally there is an apparent difference in the pattern of
#' growth between first and second rotations, where a compartment may have some
#' residual fertilizer left-over from the first rotation.
#'
#' In general the compartment names began receiving the 200 series designation
#' in the 1981 financial year, however some vintage first rotation compartments
#' received a 200 series designation and this is accounted for by the function.
#' One final edge case occurs in YUROL logging area, where first rotation
#' plantings received a 200 series designation up to the 1984 financial year.
#'
#' @param cpt a character vector containing the compartment names.
#' @param pltfinyr a numeric vector containing the financial year in which the
#'   compartment was planted.
#' @param laname a character vector containing the logging area name.
#'
#' @return a numeric vector listing the rotation number.
#' @export
#'
#' @examples
#' get_rotation("33B", 1985, "SALTWATER")
get_rotation <- function(cpt, pltfinyr, laname){
    
    if(is.factor(cpt)){
        cpt <- as.character(cpt)
    }
    
    if(is.factor(laname)){
        laname <- as.character(laname)
    }
    
    
    if(!is.character(cpt)){
        stop("'cpt' must be a character vector of compartment ID's")
    }
    
    if(!is.numeric(pltfinyr)){
        stop("'pltfinyr' must be a numeric vector of financial year of planting")
    }
    
    if(!is.character(laname)){
        stop("'laname' must be a character vector of logging area names")
    }
    
    
    cpt <- formatC(cpt, width = 4)
    cpt <- gsub("\\s", "0", cpt)
    
    rotation <- vapply(cpt, substring, character(1), 1, 1)
    rotation <- as.numeric(rotation)
    
    too.old  <- pltfinyr < 1981
    yurol    <- (toupper(laname) == "YUROL") & (pltfinyr < 1984)
    
    rotation <- ifelse(rotation < 2, 1, rotation)
    rotation <- ifelse(((rotation >= 2) & too.old) | yurol, 1, rotation)
    return(rotation)
}




#' Calculate Age from Plant Date
#'
#' \code{calculate_age} calculates the age of a sample, at the desired date,
#' based on the date that the stand was planted.
#'
#' The calculation of age is a common requirement for many of HQPlantations
#' biometrics and while this calculation is very simple, it is provided here for
#' convenience.
#'
#' @param plantdate a vector that can be coerced to a Date, corresponding to the
#'   date the stand was planted.
#' @param targetdate a vector that can be coerced to a Date, corresponding to
#'   the date the age is desired.
#'
#' @return A numeric vector containing the age (in years) of the sample.
#' @import magrittr
#'
#' @examples
#' calculate_age("1990-12-11", "2018-05-21")
calculate_age <- function(plantdate, targetdate = Sys.Date()){
    
    fmts <- c("%Y-%m-%d")
    if(!inherits(plantdate, "Date")){
        if(inherits(plantdate, "POSIXt") || is.character(plantdate)){
            plantdate <- as.Date(plantdate, tryFormats = fmts, optional = TRUE)
        } else {
            stop("Unrecognized date format!")
        }
    }
    
    if(!inherits(targetdate, "Date")){
        if(inherits(targetdate, "POSIXt") || is.character(targetdate)){
            targetdate <- as.Date(targetdate, tryFormats = fmts, optional = TRUE)
        } else {
            stop("Unrecognized date format!")
        }
    }
    
    age <- as.numeric(targetdate - plantdate) / 365.25
    age <- round(age, 1)
    return(age)
}





#' Calculate Balmod
#'
#' Balmod is a competition index, defined as the total basal area (per-hectare)
#' in trees that are larger than the subject tree normalised to the total basal
#' area (per-hectare) in the stand. NOTE: strictly speaking this function
#' calculates BAL, as Balmod includes a relative spacing adjustment, but due to
#' an unfortunate mix-up in terminology when the term was first implemented the
#' name has stuck.
#'
#' @param dbh a numeric vector containing the diameter (at 1.3m) of each tree in
#'   the sample.
#' @param prob a survival probability for each tree (defaults to 1).
#'
#' @return a numeric vector containing the normalised basal area of all trees larger than the
#' subject.
#' @export
#' @import magrittr
#'
#' @examples
#' set.seed(117)
#' dbh <- rnorm(10, 20, 2)
#' calculate_balmod(dbh)
#' calculate_balmod(dbh, c(1.9, 2.9, 4.5, 1.1, 2.3, 2.3, 3.1, 3.3, 2.5, 2.7))
calculate_balmod <- function(dbh, prob = 1){
    
    tba    <- (pi/4e4) * dbh^2 * prob
    balmod <- rep(0, length(dbh))
    
    for(tree in seq_along(dbh)){
        tree.dbh <- dbh[tree]
        balmod[tree] <- sum(tba[tree.dbh < dbh]) + sum(0.5*tba[tree.dbh == dbh])
    }
    
    balmod <- balmod / sum(tba)
    return(balmod)
}




#' Calculate Basal Area Values
#'
#' \code{calculate_dbhba} calculates the quadratic-mean DBH for the given Stand
#' Basal Area (SBA) and stocking. \code{calculate_sba} calculates the SBA for
#' the given sample DBH values and the sample area.
#'
#' @param sba the stand basal area, measured in square metres per-hectare.
#' @param stk the stocking, measured in stems per-hectare.
#' @param dbh the diameter at breast height (1.3m above ground).
#' @param area the sample area, measured in hectares.
#' @param prob a survival probability for each tree.
#'
#' @return \code{calculate_dbhba} returns the quadratic mean DBH (in
#'   centimetres), while \code{calculate_sba} returns the SBA (in square metres
#'   per-hectare).
#' @export
calculate_dbhba <- function(sba, stk){
    avg.ba <- sba / stk
    dbhba  <- sqrt(avg.ba / (pi / 4e4))
    return(dbhba)
}



#' @rdname calcualate_dbhba
#' @export
calculate_sba <- function(dbh, area, prob = 1){
    sba <- (pi / 4e4) * sum(prob * dbh^2) / area
    return(sba)
}




#' Calculate survival stocking for a series of measures.
#'
#' \code{calculate_survival_stk} is a convenience function for calculating the
#' survival stocking from a set of measures, required for mortality
#' calculations.
#'
#' @param age the age (in years) of the sample at each measure.
#' @param stk the stocking (in stems per-hectare) at each measure.
#'
#' @return the survival stocking (in stems per-hectare).
#' @export
calculate_survival_stk <- function(age, stk){
    df <- data.frame(age = age, stk = stk, stringsAsFactors = FALSE)
    df <- df[df$age >= 10, ]
    survival.stk <- if(nrow(df) > 0){
        df$stk[1]
    } else {
        NA_real_
    }
    
    return(survival.stk)
}






