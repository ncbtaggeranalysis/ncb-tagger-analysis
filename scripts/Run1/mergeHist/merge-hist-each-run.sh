#!/bin/bash
#
# runs with : bash merge-hist.sh 
#

DIR=../efficiency/HIST/user.olariu.data11_7TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/
#DIR=../efficiency/HIST/user.olariu.data12_8TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz

#DIR=../misIdProb/HIST/user.olariu.data11_7TeV.physics_JetTauEtmiss.test.181229.0819_XYZ.HIST.root.tgz/

# -----------------------------------------------------------------------------

FILE_LIST=($(find $DIR -type f -name "*.HIST.root" | grep dir))

RUN_NUMBERS_DUP=""
for file in ${FILE_LIST[@]}; do
  echo $file
  runNumber=$(echo $(basename $file) | awk -F '.' '{print $2}')
  echo "runNumber=$runNumber" 
  RUN_NUMBERS_DUP="$RUN_NUMBERS_DUP $runNumber"
done

RUN_NUMBERS_NO_DUP=""

for RUN_NUMBER in ${RUN_NUMBERS_DUP[@]}; do
  echo $RUN_NUMBER

  alreadyInTheList=0
  for run in ${RUN_NUMBERS_NO_DUP[@]}; do
    if [[ $run == $RUN_NUMBER ]]; then
      alreadyInTheList=1
      break
    fi
  done

  if [ $alreadyInTheList -eq 0 ]; then
    echo "adding: $RUN_NUMBER"
    RUN_NUMBERS_NO_DUP="$RUN_NUMBERS_NO_DUP $RUN_NUMBER"
  fi

done

if [ -f tmp.txt ]; then rm tmp.txt; fi
for run in ${RUN_NUMBERS_NO_DUP[@]}; do
  echo $run >> tmp.txt
done
RUN_NUMBERS_NO_DUP=($(cat tmp.txt | sort))
rm tmp.txt

if [ ! -f $DIR/merge-each-run ]; then mkdir $DIR/merge-each-run; fi
echo "List of run numbers without duplications:"
for run in ${RUN_NUMBERS_NO_DUP[@]}; do
  echo $run

  hadd -f $DIR/merge-each-run/$run.HIST.root $(find $DIR/ -name "*.$run.HIST.root")
done
