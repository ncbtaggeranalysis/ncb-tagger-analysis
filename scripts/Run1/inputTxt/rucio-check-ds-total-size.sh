-
# runs with: zsh rucio-check-ds-total-size.sh 
#

#dataset=data11_7TeV.physics_Background.NTUP_BKGD.grl.txt
#dataset=data12_8TeV.physics_Background.NTUP_BKGD.grl.txt

dataset=data12_8TeV.physics_Background.NTUP_BKGD.grl.txt
#dataset=data12_8TeV.physics_JetTauEtmiss.NTUP_COMMON.grl.txt

total_size=0
totalFiles=0
while read ds; do
  echo $ds
  sizestr=$(rucio list-files $ds | grep "Total size" | awk -F ': ' '{print $2}')
  echo "sizestr=$sizestr"
  size=$(echo $sizestr | awk -F ' ' '{print $1}')
  unit=$(echo $sizestr | awk -F ' ' '{print $2}')
  echo "size=$size unit=$unit"
  if [[ $unit == "kB" ]]; then
    size=$(($size/1000000))
  elif [[ $unit == "MB" ]]; then 
    size=$(($size/1000))
  elif [[ $unit == "TB" ]]; then
    size=$(($size*1000)) 
  elif [[ $unit != "GB" ]]; then
    echo "wrong unit $unit"
    exit
  fi
  printf "%5.2f GB\n" "$size"
  total_size=$(($total_size+$size))
  printf "total_size=%5.2f GB\n" "$total_size"

  nFiles=$(rucio list-files $ds | grep "Total files" | awk -F ':' '{print $2}')
  echo "nFiles=$nFiles"
  totalFiles=$(($nFiles+$totalFiles))
  #totalFiles=$(echo "$nFiles+$totalFiles" | bc)
  echo "nFiles=$nFiles totalFiles=$totalFiles"
done < $dataset
