#
# runs with : bash efficiencyTable.sh 
#

#SAMPLE=../efficiency/HIST/user.olariu.data11_7TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/merge-all/data11_7TeV.merge.HIST.root
#SYST_FILE=../efficiencyVsRunNumber/HIST/data11_7TeV.efficiencyVsRunNumber.root

SAMPLE=../efficiency/HIST/user.olariu.data12_8TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/merge-all/data12_8TeV.merge.HIST.root
SYST_FILE=../efficiencyVsRunNumber/HIST/data12_8TeV.efficiencyVsRunNumber.root

EFFICIENCY_HISTOGRAMS=(
h_ntagged_bbMoore       # MooreSegments
h_ntagged_bb            # ConvertedMBoySegments
)

#------------------------------------------------------------------------------------

if [[ $SAMPLE == *data11_7TeV* ]]; then projectTag="data11_7TeV";
elif [[ $SAMPLE == *data12_8TeV* ]]; then projectTag="data12_8TeV";
else echo "unknown project tag"; exit; fi

LOG=$projectTag.log
if [ -f $LOG ]; then rm $LOG; fi

for EFF_HIST in ${EFFICIENCY_HISTOGRAMS[@]}; do
  echo >> $LOG

  ./efficiencyTable \
  --effHistFile $SAMPLE \
  --effHistName $EFF_HIST \
  --systFile $SYST_FILE >> $LOG 

done

echo "==> $LOG"
