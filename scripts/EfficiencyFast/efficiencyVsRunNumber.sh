histDir=HIST/merge-by-period
allDataFile=HIST/merge-all/Run2_13TeV.All.efficiencyFast.HIST.root
log=efficiencyVsRunNumber.log

#-------------------------------------------------------------------------

rm efficiencyVsRunNumber.root
rm $log

./efficiencyVsRunNumber \
 --histDir $histDir \
 --allDataFile $allDataFile >> $log

echo "==> $log"
