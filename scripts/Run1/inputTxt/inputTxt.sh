#
# runs with: bash inputTxt.sh
#

#project_tag=data11_7TeV
project_tag=data12_8TeV

stream=physics_Background
NTUP=NTUP_BKGD

#stream=physics_JetTauEtmiss
#NTUP=NTUP_COMMON

grl1=../data/GRL/data11_7TeV.periodAllYear_DetStatus-v36-pro10-02_CoolRunQuery-00-04-08_All_Good.xml
grl2=../data/GRL/data12_8TeV.periodAllYear_HEAD_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml

log=$project_tag.$stream.$NTUP.grl.txt

# ------------------------------------------------------------------------------------

GoodRunList1=$(cat $grl1 | grep RunList)
GoodRunList1=${GoodRunList1/"      <Metadata Name=\"RunList\">"/}
GoodRunList1=${GoodRunList1/"</Metadata>"/}
GoodRunList1=${GoodRunList1//","/" "}

echo "GRL data11: $GoodRunList1"

for run in ${GoodRunList1[@]}; do
  echo $run
done

GoodRunList2=$(cat $grl2 | grep RunList)
GoodRunList2=${GoodRunList2/"<Metadata Name=\"RunList\">"/}
GoodRunList2=${GoodRunList2/"</Metadata>"/}
GoodRunList2=${GoodRunList2//","/" "}
echo "GRL data12: $GoodRunList2"

for run in ${GoodRunList2[@]}; do
  echo $run
done

if [ -f $log ]; then rm $log; fi

datasets=($(rucio list-dids "$project_tag.*.$stream.merge.$NTUP.*/" | grep NTUP | awk -F '|' '{print $2}' | sort))

runNum=""
for ds in ${datasets[@]}; do
  echo
  echo $ds

  continue

  reco_tag=$(echo $ds | awk -F '.' '{print $6}')
  #echo "reco_tag=$reco_tag"
  # try to exclude the fast reprocessing, some datasets seem problematic
  # like data11_7TeV:data11_7TeV.00178044.physics_Background.merge.NTUP_BKGD.x103_m763
  # which contains files that cannot be downloaded. 
  if [[ $reco_tag == x* ]]; then continue; fi
  
  RUN_NUMBER=$(echo $ds | awk -F '.' '{print $2}')
  RUN_NUMBER=$(echo $RUN_NUMBER | awk '{print substr($0,3,length($0))}')
  echo "RUN_NUMBER=$RUN_NUMBER"

  isInTheGRL=0
  for run in ${GoodRunList1[@]}; do 
    if [[ $run == $RUN_NUMBER ]]; then
      isInTheGRL=1
      break
    fi 
  done

  for run in ${GoodRunList2[@]}; do
    if [[ $run == $RUN_NUMBER ]]; then
      isInTheGRL=1
      break
    fi
  done

  echo "isInTheGRL=$isInTheGRL"

  if [ $isInTheGRL -eq 1 ]; then
    echo "Run is in the GRL."
  else
    continue
  fi

  nFiles=$(rucio list-files $ds | grep "Total files :" | awk -F ':' '{print $2}')
  echo "#files="$nFiles
  if [ $nFiles -eq 0 ]; then continue; fi 
 
  runNumOld=$runNum
  runNum=$(echo $ds | awk -F '.' '{print $2}')
  #echo "$runNumOld : $runNum"
  
  if [[ $runNum == $runNumOld ]]; then
    continue
  fi

  echo $ds >> $log
  echo "selected"
done
