## This is a regression test of all code required to re-produce the output of the dst-trees GAME program.
##



## Globals 
hagsCSV  <- "~/R/projects/GAMERoutines/tests/DST-Trees/DST-HAGs.csv"
treesCSV <- "~/R/projects/GAMERoutines/tests/DST-Trees/DST-Trees.csv"



## Attach packages
library(data.table)
library(GAMERoutines)



## Import data
hags  <- fread(hagsCSV)
trees <- fread(treesCSV)



## Temporary
test <- hags[TreeSeq == trees$TreeSeq[2]]
tree <- volpak_tree(test$HAG, test$DUB, test$TreeHt)

volpak_total_vol(test$HAG, test$DUB, test$TreeHt)

volpak_vol_to_hag(3, test$HAG, test$DUB, test$TreeHt)
volpak_vol_to_hag(6, test$HAG, test$DUB, test$TreeHt)

volpak_vol_to_tdub(15, test$HAG, test$DUB, test$TreeHt)
volpak_vol_to_tdub(7, test$HAG, test$DUB, test$TreeHt)

trees[TreeSeq == test$TreeSeq[1]]


tvt <- hags[, .(
    GAMERoutines = volpak_total_vol(HAG, DUB, TreeHt),
    Volpak = volpak_vtm(HAG, DUB, TreeHt)
), by = "TreeSeq"]

tvt <- tvt[, Diff := GAMERoutines - Volpak]



##############
# Run Checks #
##############


## Flag HAGS that exceed tree height, Those that do not have a 4.8m log
check_minimum_log <- function(tdub, hags, dubs, treeht, stump = 0.15, min.log = 4.8){
    
    ht.to.tdub <- tryCatch(volpak_get_hag(tdub, hags, dubs, treeht), error = function(e) e)
    
    if(!inherits(ht.to.tdub, "error")){
        check <- ht.to.tdub >= (min.log + stump)
    } else {
        check <- FALSE
    }

    return(check)
}


hags <- hags[, ]
check.tree <- hags[, .(
    CheckTDUB07 = check_minimum_log(7,  HAG, DUB, TreeHt),
    CheckTDUB15 = check_minimum_log(15, HAG, DUB, TreeHt),
    HAG_gt_TreeHt = any(HAG > TreeHt)
), by = TreeSeq]





#####################
# Calculate Volumes #
#####################


vols <- hags[, .(
    TVT = volpak_total_vol(HAG, DUB, TreeHt),
    TVTOB = volpak_total_vol(HAG, DOB, TreeHt),
    TDVOL07 = volpak_vol_to_tdub(tdubs = 7,  heights = HAG, diameters = DUB, total.height = TreeHt), 
    TDVOL15 = volpak_vol_to_tdub(tdubs = 15, heights = HAG, diameters = DUB, total.height = TreeHt)
), by = list(TreeSeq)]


vols <- vols[, TDVOL07 := ifelse(TDVOL07 < 0, 0, TDVOL07)]
vols <- vols[, TDVOL15 := ifelse(TDVOL15 < 0, 0, TDVOL15)]





##################################
# Compare Results with DST-Trees #
##################################


## Remove extraneous variables from the reference dataset.
common.fields <- names(vols)
trees <- trees[, ..common.fields]
trees <- setkey(trees, TreeSeq)


## Record zero volume for TDVOL fields that do not have a 4.8m log.
vols <- merge(vols, check.tree, by = "TreeSeq")
vols <- vols[, `:=`(
    TDVOL07 = ifelse(CheckTDUB07, TDVOL07, 0),
    TDVOL15 = ifelse(CheckTDUB15, TDVOL15, 0)
)]

vols <- setkey(vols, TreeSeq)
vols <- vols[, ..common.fields]



## Calculate differences
sum(abs(vols$TDVOL07 - trees$TDVOL07))
sum(abs(vols$TDVOL15 - trees$TDVOL15))
sum(abs(vols$TVT - trees$TVT))
sum(abs(vols$TVTOB - trees$TVTOB))


max(abs(vols$TDVOL07 - trees$TDVOL07))
max(abs(vols$TDVOL15 - trees$TDVOL15))
max(abs(vols$TVT - trees$TVT))
max(abs(vols$TVTOB - trees$TVTOB))











