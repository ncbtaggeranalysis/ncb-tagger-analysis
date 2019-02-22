effFile=HIST/181121.data18_13TeV.physics_Background/merge-all/data18_13TeV.All.HIST.root

#--------------------------------------------------------------------------------
log=efficiencyTable.$(basename $effFile)
log=${log/.HIST.root/.log}

if [ -f $log ]; then rm $log; fi

./efficiencyTable --effFastFile $effFile >> $log

echo "$log"
