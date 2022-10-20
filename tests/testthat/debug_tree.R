## This script is used to debug the creation of a single tree, with the intent
## of printing debug messages to a log file.
##


## User defined

VolpakTreeTXT <- "tests/testthat/volpak_tree.txt"
VpakinitTXT <- "tests/testthat/vpakinit.txt"
TreeSeq <- 30861




## Attach packages

library(volpak)
library(testthat)




## Load DST data

data(hags)




#################################
# Single Tree: Detailed Testing #
#################################


test <- hags[hags$TreeSeq == TreeSeq, ]

sink(file = VolpakTreeTXT)

# tree <- volpak_tree(test$HAG, test$DUB, test$TreeHt)
tree <- volpak_tree(test$HAG, test$DUB, 0)

sink()




sink(file = VpakinitTXT)

volpak_vpakinit(test$HAG, test$DUB, test$TreeHt)

sink()






