pickle_files=($(find ../../source/NCBanalysis/data/RunQueryPickleTool/ -name 270588.pickle))

for pickle in ${pickle_files[@]}; do
  echo $pickle
  python pickle2ROOT.py $pickle
done

mv -v *.root ../../source/NCBanalysis/data/RunQueryPickleTool/
