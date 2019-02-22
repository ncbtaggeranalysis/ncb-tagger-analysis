# #!/bin/zsh
# runs with: zsh makeFileList.sh   

if [[ $1 != "" ]]; then
  DS=$1
else
  DS=data18_13TeV.00348885.physics_Main.merge.AOD.f937_m1972
fi

if [[ $2 != "" ]]; then
  useGRL=$2
else
  useGRL="GRL_OFF"
fi

if [[ $3 != "" ]]; then
  MAX_TOTAL_SIZE=$3
  MAX_TOTAL_SIZE=${MAX_TOTAL_SIZE/GB/} 
else
  MAX_TOTAL_SIZE=50
fi

echo $0
echo "DS=$DS"
echo "useGRL=$useGRL"
echo "MAX_TOTAL_SIZE=$MAX_TOTAL_SIZE"



GRL=../../source/NCBanalysis/data/GRL/data15_13TeV.periodAllYear_HEAD_DQDefects-00-02-02_PHYS_StandardGRL_All_Good.xml,../../source/NCBanalysis/data/GRL/data16_13TeV.periodAllYear_HEAD_DQDefects-00-02-04_PHYS_StandardGRL_All_Good.xml,../../source/NCBanalysis/data/GRL/data17_13TeV.periodAllYear_HEAD_Unknown_PHYS_StandardGRL_All_Good.xml,../../source/NCBanalysis/data/GRL/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml

#-----------------------------------------------------------------------------

RUN_NUMBER=$(echo $DS | awk -F '.' '{print $2}')

ds_file_list=($(rucio list-files $DS | grep lb | awk -F '|' '{print $2}'))
ds_file_sizes=($(rucio list-files $DS | grep lb | awk -F '|' '{print $5}' | awk '{print $1","$2}'))

N1=${#ds_file_list[@]}
N2=${#ds_file_sizes[@]}

if [[ $N1 != $N2 ]]; then echo "array sizes do not match"; fi
echo "#files in dataset=$N1"

file_counter=0

total_size=0
let imax=$N1-1
for i in $(seq 0 $imax); do
  file=${ds_file_list[$i]}
  filesize_str=${ds_file_sizes[$i]} 
  echo "$i $file $filesize_str" 
  if [[ $filesize_str != *GB ]]; then continue; fi 
  filesize=${filesize_str/,GB/}

  lb_str=$(echo $file | awk -F '.' '{print $7}')
  if [[ $lb_str != _lb* ]]; then
    echo "lb_str should start with _lb"
    return
  fi
  lb=$(echo $lb_str | awk '{print substr($0,4,length($0))}')

  if [[ $useGRL == "GRL_ON" ]]; then
    ./runLBisInGRL --GRL $GRL --runNumber $RUN_NUMBER --lb $lb
    sc=$?
  fi
  if [[ $useGRL == "GRL_OFF" ]] || [ $sc -eq 1 ]; then
    total_size=$(($total_size+$filesize))
    total_size=$(printf "%i" $total_size)
    echo "==>" $file 
  fi

  echo "$RUN_NUMBER lb=$lb isInGRL=$sc total=$total_size GB"

  if [ $total_size -gt $MAX_TOTAL_SIZE ]; then break; fi 
done

