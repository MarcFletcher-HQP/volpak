## code to prepare `DATASET` dataset goes here

usethis::use_data(hags, overwrite = TRUE)




hags <- read.csv("data-raw/DST-HAGs.csv", header = TRUE)



saveRDS(object = hags, file = "data/DST-HAGs.rd")
