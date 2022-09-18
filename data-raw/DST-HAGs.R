## code to prepare `DATASET` dataset goes here

usethis::use_data(hags, overwrite = TRUE)




hags <- read.csv("data-raw/DST-HAGs.csv", header = TRUE)
hags <- hags[hags$SppCode == 1872, ]
hags <- hags[hags$MeasYear < 2016, ]



saveRDS(object = hags, file = "data/DST-HAGs.rd")
