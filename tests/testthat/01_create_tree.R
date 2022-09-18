## This script tests the creation of a 'volpak_tree'
##



## Attach packages

library(volpak)
library(testthat)




## Load DST data

data(hags)





context("Test creation of volpak_tree")



test_that("Creation of volpak_tree succeeds", {


  tree <- hags[hags$TreeSeq == hags$TreeSeq[1], ]

  expect_success(volpak_tree(hags$HAG, hags$DUB, hags$TreeHt))

})


