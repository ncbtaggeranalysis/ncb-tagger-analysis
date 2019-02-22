DS_LIST_VEC=(
inputTxt/v181121/data16_13TeV.physics_Background.txt
inputTxt/v181121/data17_13TeV.physics_Background.txt
#inputTxt/v181121/data18_13TeV.physics_Background.txt
#inputTxt/v181121/data18_13TeV.00348197.physics_Main.AOD.txt
)

RUN_TAGGER_ANALYSIS=1
RUN_TAGGER_ANALYSIS_FAST=0

#OUT_DIR=HIST/test
OUT_DIR=HIST/WorkDir

FILTER=
EXCLUDED=

MAX_PROCESSES=-1
#MAX_PROCESSES=128
#MAX_PROCESSES=$(cat /proc/cpuinfo  | grep processor | wc -l)

# ------------------------------------------------------------------

if [ ! -d $OUT_DIR ]; then mkdir -v $OUT_DIR; fi

for DS_LIST in ${DS_LIST_VEC[@]}; do

  while read -r ds; do
    if [[ ${ds:0:1} == '#' ]] || [[ $ds != *$FILTER* ]] ; then continue; fi
    if [[ $EXCLUDED != "" ]] && [[ $ds == *$EXCLUDED* ]]; then continue; fi
    echo $ds
 
    if [ $MAX_PROCESSES != -1 ]; then 
      while : 
      do
        nProcesses=$(ps au  | grep taggerAnalysis | wc -l)
        if [ $nProcesses -lt $MAX_PROCESSES ]; then
          break
        else
          echo "Waiting: #taggerAnalysis processes=$nProcesses" 
          sleep 60
        fi
      done
    fi

    if [ $RUN_TAGGER_ANALYSIS -eq 1 ]; then
      LOG=log/taggerAnalysis.$(basename $(dirname $ds)).log
      ./taggerAnalysis --inDS $ds --outDir $OUT_DIR --EvtMax -1 > $LOG &
    fi

    if [ $RUN_TAGGER_ANALYSIS_FAST -eq 1 ]; then
      LOG=log/taggerAnalysisFast.$(basename $(dirname $ds)).log 
      ./taggerAnalysisFast --inDS $ds --outDir $OUT_DIR > $LOG &
    fi

  done < $DS_LIST

done
