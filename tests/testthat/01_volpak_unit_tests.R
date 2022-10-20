## This script is a unit test of the functions provided in this R package.
##


## User defined

TestTreeSeq <- 29740




## Attach packages

library(volpak)
library(testthat)




## Load DST data

data(hags)

treeData <- hags[hags$TreeSeq == TestTreeSeq, ]




## Tree Creation

test_that("volpak_tree creates a Tree without Error", {

  expect_silent(tree <- volpak_tree(treeData$HAG, treeData$DUB, treeData$TreeHt))

  expect_s4_class(tree, "volpak_tree")
  expect_true(inherits(slot(tree, "xptr"), "externalptr"))

})




test_that("volpak_tree is freed without crashing R session", {

  expect_silent(tree <- volpak_tree(treeData$HAG, treeData$DUB, treeData$TreeHt))

  expect_silent(rm(list = "tree"))
  expect_silent(gc())

})




test_that("volpak_tree is created when no tree height is available", {

  expect_silent(tree <- volpak_tree(treeData$HAG, treeData$DUB, 0))

  expect_s4_class(tree, "volpak_tree")
  expect_true(inherits(slot(tree, "xptr"), "externalptr"))

})







