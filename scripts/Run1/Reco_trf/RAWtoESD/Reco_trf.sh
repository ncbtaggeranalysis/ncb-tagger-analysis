inputBSFile=$STORAGE/data11_7TeV.00191933.physics_JetTauEtmiss.merge.RAW/data11_7TeV.00191933.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-11._0001.1 

outputESDFile=data11_7TeV.00191933.physics_JetTauEtmiss.ESD.pool.root

#----------------------------------------------------------------------------

if [ -f $outputESDFile ]; then rm $outputESDFile; fi

Reco_trf.py \
inputBSFile=$inputBSFile \
maxEvents=-1 \
autoConfiguration=everything \
outputESDFile=$outputESDFile
