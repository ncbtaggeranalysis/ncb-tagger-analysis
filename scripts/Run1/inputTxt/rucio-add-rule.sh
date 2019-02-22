#
# runs with zsh rucio-add-rule.sh
#

#inputTxt=data11_7TeV.physics_Background.NTUP_BKGD.grl.txt
#inputTxt=data12_8TeV.physics_Background.NTUP_BKGD.grl.txt

#inputTxt=data11_7TeV.physics_JetTauEtmiss.NTUP_COMMON.grl.txt
inputTxt=data12_8TeV.physics_JetTauEtmiss.NTUP_COMMON.grl.txt

SE=CERN-PROD_SCRATCHDISK

MAX_DATASETS=-1
DATASET_FRACTION=20

#---------------------------------------------------------------------

ds_list=($(cat $inputTxt))
counter=0

for ds in ${ds_list[@]}; do
  echo $ds

  if [[ $ds == \#* ]]; then continue; fi 

  rucio list-dataset-replicas $ds
  #continue

  rucio list-dataset-replicas $ds | grep $SE
  if [ $? -eq 0 ]; then 
    echo "Skip: existing replica found at $SE"
    continue 
  fi
  
  if [ $DATASET_FRACTION -eq -1 ] || [ $(($counter % $DATASET_FRACTION)) -eq 0 ]; then
    CMD="rucio add-rule $ds 1 CERN-PROD_SCRATCHDISK"
    echo $CMD
    eval $CMD
  fi
  let counter=$counter+1

  if [ $MAX_DATASETS -ne -1 ] && [ $counter -ge $MAX_DATASETS ]; then 
    break
  fi
done 
