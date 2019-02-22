log=checkLArTileError.log.data18_13TeV
if [ -f $log ]; then rm $log; fi
./checkLArTileError inputTxt/v181121/data18_13TeV.physics_Background.txt >> $log
