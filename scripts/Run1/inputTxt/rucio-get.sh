datasets=($(cat make-ds-list.log))
DEST_DIR=$HOME/workspace/data
MAX_DATASETS=20

ds_counter=0
for ds in $datasets; do
  echo $ds
  rucio get --dir $DEST_DIR $ds 

  #let ds_counter=$ds_counter+1
  #if [ $ds_counter -ge $MAX_DATASETS ]; then
  #  break
  #fi

  du -ch
  diskusage=$(\du -c $HOME/workspace/data | tail -n 1 | awk '{print $1}')
  if [ $diskusage -gt 50000000 ]; then
    echo "exceeded 50GB of diskspace, exit."
    break;
  else
    echo "diskusage so far: $diskusage k"
  fi

done
