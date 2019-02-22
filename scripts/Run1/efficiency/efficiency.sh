DIR=$STORAGE/data11_7TeV.00191933.physics_Background.merge.NTUP_BKGD.f414_p1196

if [ -f input.txt ]; then rm input.txt; fi
find $DIR -name "*root*" -exec printf "%s," '{}' \; >> input.txt

export sampleDir=""
export inputTxt=input.txt
export makeCutflowHist=1
root -b -q efficiency.C+

