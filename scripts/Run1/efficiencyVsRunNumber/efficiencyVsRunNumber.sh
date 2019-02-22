#
# runs with : bash efficiencyVsRunNumber.sh 
#

dirList=(
../efficiency/HIST/user.olariu.data11_7TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/merge-each-period
#../efficiency/HIST/user.olariu.data12_8TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/merge-each-run/
../efficiency/HIST/user.olariu.data12_8TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/merge-each-period/
)

EFFICIENCY_HISTOGRAMS=(
h_ntagged_bbMoore       # MooreSegments
h_ntagged_bb            # ConvertedMBoySegments
)

LOG=log.txt

if [ -f $LOG ]; then rm $LOG; fi

for dir in ${dirList[@]}; do

  echo $dir
  if [ -f input.txt ]; then rm input.txt; fi 
  find $dir -type f -name "*.HIST.root" >> input.txt

  for effHist in ${EFFICIENCY_HISTOGRAMS[@]}; do
  ./efficiencyVsRunNumber input.txt $effHist >> $LOG
  done

done

echo "==> $LOG"
