#!/bin/bash

DS_LIST_VEC=(
#
# beamhalo samples in unpaired bunches 
#
#../inputTxtNtuple/181121/data16_13TeV.physics_Background.txt
#../inputTxtNtuple/181121/data17_13TeV.physics_Background.txt
#../inputTxtNtuple/181121/data18_13TeV.physics_Background.txt
#../inputTxtNtuple/181121/data18_13TeV.00348197.physics_Main.AOD.txt
#../inputTxtNtuple/190203-mc/beambackground-mc.txt
#
# dijet samples in colliding bunches
#
../inputTxtNtuple/190211/mc16_13TeV.jetjet.txt
#../inputTxtNtuple/190211/dataRun2_13TeV.physics_Main.txt
)

#SAMPLE_TYPE=beamhalo
SAMPLE_TYPE=dijet

FILTER=
EXCLUDED=

MAX_RUNS=-1
MAX_JOBS_IN_PARALLEL=10

#UPDATE_EXISTING=1

DO_JETS=1
# ------------------------------------------------------------------

run_counter=0
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
    echo $ds

    #existingOutputFile=$(find $OUT_DIR -name *.$runNumber.*.root | wc -l) 
    #if [ $UPDATE_EXISTING -eq 0 ] && [ $existingOutputFile -eq 1 ]; then
    #  echo "Output file already existing, skippinig"
    #  continue
    #fi

    while :
    do
      njobs=$(ps aux | grep efficiencyFast | wc -l)
      let njobs=$njobs-1
      if [ $njobs -le $MAX_JOBS_IN_PARALLEL ]; then
        break
      else
        echo "sleep 10"
        sleep 10
      fi
    done

    dsDir=$(dirname $ds)
    dsDirBase=$(basename $dsDir)
    if [ ! -d $OUT_DIR/log ]; then mkdir $OUT_DIR/log; fi
    log=$OUT_DIR/log/$dsDirBase.log
    if [ -f $log ]; then rm $log; fi
    cmd="./efficiencyFast \
         --inDS $ds \
         --outDir $OUT_DIR \
         --sampleType $SAMPLE_TYPE \
         --doJets $DO_JETS \
         >> $log &"
    echo $cmd
    eval $cmd 

    jobids="$jobids $!"
    let run_counter=$run_counter+1
    if [ $MAX_RUNS -ne -1 ] && [ $run_counter -ge $MAX_RUNS ]; then break; fi
  done < $DS_LIST
  if [ $MAX_RUNS -ne -1 ] && [ $run_counter -ge $MAX_RUNS ]; then break; fi
done

while :
do
  njobs=0
  for jobid in ${jobids[@]}; do
    isRunning=$(ps aux | grep efficiencyFast | grep $jobid | wc -l)
    if [ $isRunning -eq 1 ]; then
      let njobs=$njobs+1
    fi
  done
  echo "running jobs: $njobs"
  if [ $njobs -eq 0 ]; then break; fi
  sleep 10
done     
