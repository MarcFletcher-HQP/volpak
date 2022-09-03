check_input_length <- function(...){
    
    mc   <- list(...)
    nObs <- vapply(mc, function(x) length(eval(x)), numeric(1), USE.NAMES = TRUE)
    
    bad.length <- !((nObs == 1) | (nObs == max(nObs)))
    if(any(bad.length)){
        msg <- which(bad.length) %>% 
            extract(names(nObs), .) %>% 
            paste(collapse = ", ") %>% 
            gsub(",(?!.*,)", " &", ., perl = TRUE)
        
        stop("Length mis-match for the following arguments: ", msg, 
             "\n length must be either 1 or ", max(nObs))
    }
    
    return(max(nObs))
}
