
inputESDFile=../RAWtoESD/data11_7TeV.00191933.physics_JetTauEtmiss.ESD.pool.root

# -----------------------------------------------------------------------------

inputESDFileBase=$(basename $inputESDFile)
outputNTUP_BKGDFile=${inputESDFileBase/ESD/NTUP_BKGD}

if [ -f $outputNTUP_BKGDFile ]; then rm $outputNTUP_BKGDFile; fi

Reco_trf.py \
inputESDFile=$inputESDFile \
maxEvents=-1 \
autoConfiguration=everything \
outputNTUP_BKGDFile=$outputNTUP_BKGDFile
