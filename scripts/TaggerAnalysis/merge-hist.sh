PROJECT_TAGS=(
#data16_13TeV
data17_13TeV
data18_13TeV
)

#DIR=HIST/test
DIR=HIST/WorkDir

STREAM=physics_Background

ALL_YEAR_TAG=AllYear

MERGE_EACH_YEAR=0
MERGE_EACH_PERIOD=1

#-----------------------------------------------------
CWD=$PWD
cd $DIR/merge
for project_tag in ${PROJECT_TAGS[@]}; do

  if [ $MERGE_EACH_YEAR -eq 1 ]; then
    if [[ $project_tag == data16_13TeV ]]; then
      hadd -f data16_13TeV.AllYearNo307124.$STREAM.merge.HIST.root $(ls ../data16_13TeV.*.$STREAM.HIST.root | grep -v 307124)
    else
      hadd -f $project_tag.$ALL_YEAR_TAG.$STREAM.merge.HIST.root ../$project_tag.*.$STREAM.HIST.root
    fi
  fi

  if [ $MERGE_EACH_PERIOD -eq 1 ]; then
    if [[ $project_tag == data16_13TeV ]]; then
      periods=(periodA periodB periodC periodD periodE periodF periodG periodI periodJ periodK periodL)
    elif [[ $project_tag == data17_13TeV ]]; then
      periods=(periodA periodB periodC periodD periodE periodF periodG periodH periodI periodK periodN)
    elif [[ $project_tag == data18_13TeV ]]; then
      periods=(periodA periodB periodC periodD periodE periodF periodG periodH periodI periodJ periodK periodL periodM periodN periodO periodQ)
    else
      echo "unknown project tag"; return
    fi

    for period in ${periods[@]}; do
      hadd -f $project_tag.$period.$STREAM.merge.HIST.root ../$project_tag.*.$period.$STREAM.HIST.root
    done
  fi

done
