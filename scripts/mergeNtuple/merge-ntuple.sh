#!/bin/bash

DATA_DIR=$STORAGE/NCB/NCBTaggerAnalysis/190206
MAX_JOBS_IN_PARALLEL=5

#-----------------------------------------------------------

if [[ $1 == "clean-job-data" ]]; then
  find $DATA_DIR -name "user.*.root" -exec rm '{}' \;
  exit
elif [[ $1 == "clean-merged-files" ]]; then
  find $DATA_DIR -name "NtupleWriter.merge.root" -exec rm '{}' \;
  exit
fi

run_dirs=($(find $DATA_DIR -type d -name "user.*"))
jobids=""
for run_dir in ${run_dirs[@]}; do
  echo
  echo $run_dir

  files=($(find $run_dir -name "*.root*" -type f))
  nfiles=${#files[@]}
  if [ $nfiles -eq 0 ]; then continue; fi

  while :
  do
    njobs=$(ps aux | grep hadd | wc -l)
    let njobs=$njobs-1
    if [ $njobs -le $MAX_JOBS_IN_PARALLEL ]; then
      break
    else
      echo "sleep 10 ..."
      sleep 10
    fi
  done

  if [ -f $run_dir/NtupleWriter.merge.root ]; then 
    rm $run_dir/NtupleWriter.merge.root 
  fi

  echo "#files=${#files[@]}"
  hadd -f $run_dir/NtupleWriter.merge.root $run_dir/*.root >> /dev/null &
  jobids="$jobids $!"  
done 

while :
do
  njobs=0
  for jobid in ${jobids[@]}; do
    ps aux | grep hadd | grep $jobid
    if [ $? -eq 0 ]; then
      njobs=$njob+1
    fi
  done
  echo "running hadd jobs: $njobs"
  if [ $njobs -eq 0 ]; then break; fi
  sleep 10
done
