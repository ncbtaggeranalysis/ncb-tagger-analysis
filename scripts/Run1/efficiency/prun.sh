#!/bin/bash
#
# runs with: bash prun.sh
#

inputTxt=(
#data11_7TeV.physics_Background.test.txt
data11_7TeV.physics_Background.NTUP_BKGD.grl.txt
#
data12_8TeV.physics_Background.NTUP_BKGD.grl.txt.00
data12_8TeV.physics_Background.NTUP_BKGD.grl.txt.01
data12_8TeV.physics_Background.NTUP_BKGD.grl.txt.02
)

TAG=$(date +%y%m%d.%H%M)

#In case of data12_8TeV.physics_Background.NTUP_BKGD.grl.txt got "exhausted job".
#Many of the datasets have just one file and many jobs are sent (~8400),
#with only 1 file / job (dataset contains ~8400 files).
#Split the txt file in few pieces.

for inputTxt in ${inputTxt[@]}; do
  echo $inputTxt
  prun \
  --excludeFile "./HIST/*,*.HIST.*,*.so,*.d,*.pcm" \
  --inDsTxt $inputTxt \
  --site ANALY_CERN \
  --rootVer 6.14/04 \
  --cmtConfig x86_64-slc6-gcc62-opt \
  --outDS user.$(whoami).${inputTxt/.txt/}.$TAG \
  --exec 'echo %IN > input.txt; cat %IN; export inputTxt=input.txt; root -b -q efficiency.C+' \
  --outputs "*.HIST.root"
done
