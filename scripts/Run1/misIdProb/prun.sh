#inputTxt=data11_7TeV.physics_JetTauEtmiss.test.txt
inputTxt=data11_7TeV.physics_JetTauEtmiss.NTUP_COMMON.grl.txt

TAG=$(date +%y%m%d.%H%M)

prun \
--inDsTxt $inputTxt \
--site ANALY_CERN \
--rootVer 6.14/04 \
--cmtConfig x86_64-slc6-gcc62-opt \
--outDS user.$(whoami).${inputTxt/.txt/}.$TAG \
--exec 'echo %IN > input.txt; cat %IN; export inputTxt=input.txt; root -b -q misIdProb.C+' \
--outputs "*.HIST.root"
