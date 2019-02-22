
beamhaloNominalConfigs=(
"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&!cut_jetCln_noise"

"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&!cut_jetCln_noise_EMF"

)

effFile=HIST/181121.dataRun2_13TeV.physics_Background/dataRun2_13TeV.merge.root 

#-----------------------------------------------------------------------------
effFileBase=$(basename $effFile)
effFileDirName=$(dirname $effFile)
effFileDirNameBase=$(basename $effFileDirName)
LOG=effTable.$effFileDirNameBase.$effFileBase.log
if [ -f $LOG ]; then rm $LOG; fi

for config in ${beamhaloNominalConfigs[@]}; do
  printf "$config\n\n"
  ./effTable --effFile $effFile --nominalConfig $config >> $LOG
  if [ $? -ne 0 ]; then echo "exit with error status code"; exit; fi
done

echo "$LOG"
