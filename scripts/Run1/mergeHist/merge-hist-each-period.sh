#!/bin/bash
#
# runs with : bash merge-hist-by-period.sh 
#

DIR=../efficiency/HIST/user.olariu.data11_7TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/
#DIR=../efficiency/HIST/user.olariu.data12_8TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/

# -------------------------------------------------------------------------

if [ ! -d $DIR ]; then echo "$DIR does not exist"; exit; fi

periods=(A B C D E F G H I J K L M N O P Q)

HIST_FILES=($(find $DIR/merge-each-run . -maxdepth 1 -name "*.HIST.root"))

if [ ! -d $DIR/merge-each-period ]; then mkdir $DIR/merge-each-period; fi

if [[ $DIR == *data11_7TeV* ]]; then PROJECT_TAG=data11_7TeV
elif [[ $DIR == *data12_8TeV* ]]; then PROJECT_TAG=data12_8TeV
else echo "unknown project tag"; exit; fi

for period in ${periods[@]}; do
  echo $period
  fileListOnePeriod=""
  for histfile in ${HIST_FILES[@]}; do
    echo $histfile
    histfilebase=$(basename $histfile)
    runNum=$(echo $histfilebase | awk -F '.' '{print $1}')
    ./runIsInDataTakingPeriod $runNum $period  
    sc=$?
    echo "$runNum sc=$sc"
    if [ $sc -eq 1 ]; then fileListOnePeriod="$fileListOnePeriod $histfile"; fi
  done
  numRuns=0
  hadd_cmd="hadd -f $DIR/merge-each-period/$PROJECT_TAG.period$period.HIST.root "
  for file in ${fileListOnePeriod[@]}; do
    echo $file
    let numRuns=$numRuns+1
    hadd_cmd="$hadd_cmd $file"
  done
  echo "numRuns=$numRuns"
  if [ $numRuns -eq 0 ]; then continue; fi
  echo $hadd_cmd
  eval $hadd_cmd
  if [ $? -ne 0 ]; then echo "non-zero sc from hadd"; exit; fi  
done



