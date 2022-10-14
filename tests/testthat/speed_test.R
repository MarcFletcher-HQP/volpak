## This script performs a speed test of the new volpak code, against the
## original C code. Note that the C code is not being run in an optimal fashion.
## 
## A dedicated speed test function should probably be written.
##


## User defined

Trees <- 1e4




## Attach packages

library(volpak)
library(testthat)




## Load DST data

data(hags)




## Resample DST data

treeid <- sample(x = unique(hags$TreeSeq), size = Trees, replace = TRUE)




## New volpak code

system.time({
  for(x in treeid){
    df <- hags[hags$TreeSeq == x, ]
    tree <- volpak_tree(df$HAG, df$DUB, df$TreeHt)
    vol <- volpak_total_vol(tree, TRUE)
    voltd07 <- volpak_vol_to_tdub(tree, 7, TRUE)
    voltd15 <- volpak_vol_to_tdub(tree, 15, TRUE)
  }
})



## Old volpak code

system.time({
  for(x in treeid){
    df <- hags[hags$TreeSeq == x, ]
    vol <- volpak_vtm(df$HAG, df$DUB, df$TreeHt)
    voltd07 <- volpak_vold(7, df$HAG, df$DUB, df$TreeHt)
    voltd15 <- volpak_vold(15, df$HAG, df$DUB, df$TreeHt)
  }
})






