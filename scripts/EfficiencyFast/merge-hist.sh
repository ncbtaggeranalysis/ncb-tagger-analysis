#!/bin/bash

HIST_DIR=HIST/190211.dataRun2_13TeV.physics_Main
#HIST_DIR=HIST/181121.data18_13TeV.physics_Background

PROJECT_TAGS=("data16_13TeV" "data17_13TeV" "data18_13TeV")
PERIODS=(A B C D E F G H I J K L M N O P Q)

#-----------------------------------------------------------------------------

if [[ $1 == "merge-all" ]]; then
  if [ ! -d $HIST_DIR/merge-all ]; then mkdir $HIST_DIR/merge-all; fi
  hadd -f $HIST_DIR/merge-all/dataRun2_13TeV.All.HIST.root \
          $HIST_DIR/data*_13TeV.*.period*.HIST.root
  exit

elif [ $1 == "merge-each-period" ]]; then
  for project_tag in ${PROJECT_TAGS[@]}; do
    echo $project_tag
    for period in ${PERIODS[@]}; do
      nfiles=$(find $HIST_DIR -name "$project_tag.*.period$period.*.root" | wc -l)
      if [ $nfiles -ne 0 ]; then
        echo "$period nFiles=$nfiles"
        hadd -f $HIST_DIR/merge-by-period/$project_tag.period$period.efficiencyFast.HIST.root \
                $HIST_DIR/$project_tag.00*.period$period.efficiencyFast.HIST.root
      fi
    done
  done
  exit
fi
