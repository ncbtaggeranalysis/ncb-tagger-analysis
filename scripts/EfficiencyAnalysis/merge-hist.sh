#!/bin/bash

#DIR=HIST/181121.dataRun2_13TeV.physics_Background/181121.data16_13TeV.physics_Background
DIR=HIST/181121.dataRun2_13TeV.physics_Background/181121.data17_13TeV.physics_Background/

#DIR=HIST/190211.dataRun2_13TeV.physics_Main

#FILTER=data
#FILTER=data16_13TeV
FILTER=data17_13TeV
#FILTER=data18_13TeV

#----------------------------------------------------------------------------------

if [[ $1 == "onejob" ]]; then
  if [ ! -d $DIR/merge ]; then mkdir $DIR/merge; fi
  hadd -f $DIR/merge/$FILTER.merge.root $(find $DIR -maxdepth 1 -name "$FILTER*.root" | sort )
  exit

elif [[ $1 == "data16_data18" ]]; then
  hadd -f $DIR/merge/data16_data18_13TeV.merge.root $DIR/merge/data16_13TeV.merge.root $DIR/merge/data18_13TeV.merge.root
  exit
fi

if [[ $1 == "parallel" ]]; then

  FILES=($(find $DIR -maxdepth 1 -name "$FILTER*.root" | sort))
  nfiles=${#FILES[@]}

  ncpus=$(cat /proc/cpuinfo  | grep processor | wc -l)
  nfiles_per_job=$((2*$nfiles/$ncpus))
  echo "nfiles_per_job=$nfiles_per_job"
  
  file_counter=0
  job_counter=0
  job_file_list=""

  if [ ! -d $DIR/merge ]; then mkdir $DIR/merge; fi

  for file in ${FILES[@]}; do
    let file_counter=$file_counter+1
    echo
    echo "$((file_counter%$nfiles_per_job)) $file"

    if [ $((file_counter%$nfiles_per_job)) -eq 0 ] || [ $file_counter -eq $nfiles ]; then
      if [ $file_counter -eq $nfiles ]; then job_file_list="$job_file_list $file"; fi        
      hadd_cmd="hadd -f $DIR/merge/merge_$job_counter.root $job_file_list"
      echo $hadd_cmd
      eval $hadd_cmd >> /dev/null &

      job_file_list=$file
      let job_counter=$job_counter+1
    else
      job_file_list="$job_file_list $file"
    fi
  done

  while :
  do
    n=$(ps aux | grep hadd | wc -l)
    let n=$n-1
    echo "running hadd jobs: $n"
    if [ $n -eq 0 ]; then break; fi
    sleep 10
  done

  hadd -f $DIR/merge/$FILTER.merge.root $DIR/merge/merge_*.root
  rm $DIR/merge/merge_*

fi
