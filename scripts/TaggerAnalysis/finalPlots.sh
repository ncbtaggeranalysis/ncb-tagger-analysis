
UNPAIRED_SAMPLE_LIST=(
merge/data16_13TeV.AllYearNo307124.physics_Background.merge.HIST.root
merge/data17_13TeV.AllYear.physics_Background.merge.HIST.root
merge/data18_13TeV.AllYear.physics_Background.merge.HIST.root
)

for unpaired_sample in ${UNPAIRED_SAMPLE_LIST[@]}; do
  PLOTS_FILE=${unpaired_sample/merge\//}
  PLOTS_FILE=${PLOTS_FILE/.physics_Background/}
  PLOTS_FILE=${PLOTS_FILE/.merge/}
  PLOTS_FILE=${PLOTS_FILE/.HIST.root/}

  UNPAIRED_SAMPLE=../TaggerAnalysis/HIST/WorkDir/$unpaired_sample
  COLLISION_SAMPLE=../TaggerAnalysis/HIST/WorkDir/data18_13TeV.00348197.periodA.physics_Main.HIST.root

  if [ ! -f $UNPAIRED_SAMPLE ]; then echo "unpaired sample missing"; return; fi
  if [ ! -f $COLLISION_SAMPLE ]; then echo "collision sample missing"; return; fi

  ./finalPlots \
  --unpaired-sample $UNPAIRED_SAMPLE \
  --collision-sample $COLLISION_SAMPLE \
  --plotsFile finalPlots.$PLOTS_FILE.pdf 
  if [ $? -ne 0 ]; then return; fi
done

