## This script tests the creation of a 'volpak_tree'
##



## Attach packages

library(volpak)
library(testthat)




## Load DST data

data(hags)






neiloidstump <- hags[hags$TreeSeq == hags$TreeSeq[1], ]

sink(file = "volpak_tree.txt")
tree <- volpak_tree(neiloidstump$HAG, neiloidstump$DUB, neiloidstump$TreeHt)
sink()

sink(file = "volpak_vpakinit.txt")
volpak_vpakinit(neiloidstump$HAG, neiloidstump$DUB, neiloidstump$TreeHt)
sink()


volpak_total_vol(tree, TRUE)

