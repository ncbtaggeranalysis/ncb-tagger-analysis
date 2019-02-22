#!/bin/bash

DS_LIST_VEC=(
#
# efficiency samples in unpaired bunches
#
#../inputTxtNtuple/181121/data16_13TeV.physics_Background.txt
#../inputTxtNtuple/181121/data17_13TeV.physics_Background.txt
#../inputTxtNtuple/181121/data18_13TeV.physics_Background.txt
#../../TaggerAnalysis/inputTxt/v181121/data18_13TeV.00348197.physics_Main.AOD.txt
# 
#
# mis-id probability samples
#
#../inputTxtNtuple/190211/dataRun2_13TeV.physics_Main.txt
../inputTxtNtuple/190211/mc16_13TeV.jetjet.txt
)

#SAMPLE_TYPE="beamhalo"
SAMPLE_TYPE="dijet"

FILTER=
#FILTER=periodB

EXCLUDED=

MAX_JOBS_IN_PARALLEL=10

evtMax=-1
#evtMax=100000

UPDATE_EXISTING=1

# ------------------------------------------------------------------

jobids=""
for DS_LIST in ${DS_LIST_VEC[@]}; do

  dsListBase=$(basename $DS_LIST)
  dsListDirName=$(dirname $DS_LIST)
  dsListDirNameBase=$(basename $dsListDirName)
  OUT_DIR=HIST/$dsListDirNameBase.${dsListBase/.txt/}
  if [ ! -d $OUT_DIR ]; then mkdir -v $OUT_DIR; fi

  while read -r ds; do
    if [[ ${ds:0:1} == '#' ]] || [[ $ds != *$FILTER* ]] ; then continue; fi
    if [[ $EXCLUDED != "" ]] && [[ $ds == *$EXCLUDED* ]]; then continue; fi
    echo
    echo $ds
  
    ds_tmp=($(echo $ds | tr "." "\n"))
    projectTagFound=0
    for field in ${ds_tmp[@]}; do
      if [ $projectTagFound -eq 1 ]; then break; fi 
      if [[ $field == *_13TeV ]]; then
        projectTagFound=1
      fi
    done
    if [ $projectTagFound -ne 1 ]; then 
      echo "failed to get run number for ds=$ds"
      exit
    fi
    runNumber=$field
    runNumber=$(printf "%.0f" "$runNumber")
    echo "runNumber=$runNumber"

    existingOutputFile=$(find $OUT_DIR -name *.$runNumber.*.root | wc -l)
    echo "existingOutputFile=$existingOutputFile"
    if [ $UPDATE_EXISTING -eq 0 ] && [ $existingOutputFile -eq 1 ]; then
      echo "Output file already existing, skippinig"
      continue
    fi

    while :
    do
      njobs=$(ps aux | grep effAnalysis | wc -l)
      let njobs=$njobs-1
      if [ $njobs -le $MAX_JOBS_IN_PARALLEL ]; then
        break
      else
        echo "sleep 10"
        sleep 10
      fi
    done 
 
    cmd="./effAnalysis \
    --sample $ds:$SAMPLE_TYPE \
    --outDir $OUT_DIR \
    --evtMax $evtMax >> /dev/null &"

    echo $cmd
    eval $cmd

    jobids="$jobids $!"

  done < $DS_LIST

done

while :
do
  njobs=0
  for jobid in ${jobids[@]}; do
    isRunning=$(ps aux | grep effAnalysis | grep $jobid | wc -l)
    if [ $isRunning -eq 1 ]; then
      let njobs=$njobs+1
    fi
  done
  echo "running jobs: $njobs"
  if [ $njobs -eq 0 ]; then break; fi
  sleep 10
done
