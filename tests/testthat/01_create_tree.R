## This script tests the creation of a 'volpak_tree'
##



## Attach packages

library(volpak)
library(testthat)




## Load DST data

data(hags)






neiloidstump <- hags[hags$TreeSeq == 29857, ]

sink(file = "volpak_tree.txt")
tree <- volpak_tree(neiloidstump$HAG, neiloidstump$DUB, neiloidstump$TreeHt)
sink()

sink(file = "volpak_vpakinit.txt")
volpak_vpakinit(neiloidstump$HAG, neiloidstump$DUB, neiloidstump$TreeHt)
sink()


volpak_total_vol(tree, TRUE)
volpak_vtm(neiloidstump$HAG, neiloidstump$DUB, neiloidstump$TreeHt)


treeid <- unique(hags$TreeSeq)
comparison <- vector("list", length(treeid))
names(comparison) <- as.character(treeid)

for(x in treeid){

  df <- hags[hags$TreeSeq == x, ]

  tree <- volpak_tree(df$HAG, df$DUB, df$TreeHt)

  tmp <- data.frame(
    TotalVol = volpak_total_vol(tree, TRUE),
    VTM = volpak_vtm(df$HAG, df$DUB, df$TreeHt),
    TDVOL07 = volpak_vol_to_tdub(7, tree, TRUE),
    VOLD07 = volpak_vold(7, df$HAG, df$DUB, df$TreeHt),
    HAG15cm = volpak_get_hag(tree, 15),
    HTD15cm = volpak_htd(15, df$HAG, df$DUB, df$TreeHt)
  )

  tmp$VOLHAG <- volpak_vol_to_hag(tree, tmp$HAG15cm, TRUE)
  tmp$VOLH <- volpak_volh(tmp$HAG15cm, df$HAG, df$DUB, df$TreeHt)

  comparison[[as.character(x)]] <- tmp

}


comparison <- do.call("rbind", comparison)
comparison$TotalVolDiff <- with(comparison, TotalVol - VTM)
comparison$TDVOL07Diff <- with(comparison, TDVOL07 - VOLD07)
comparison$HAGDiff <- with(comparison, HAG15cm - HTD15cm)
comparison$VOLHAGDiff <- with(comparison, VOLHAG - VOLH)




