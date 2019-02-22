#!/bin/bash

dsids=(361029)
#dsids=(361021 361022 361023 361024 361025 361026 361027 361028 261029 361030 361031 361032)

rm *.log

slice=1
for dsid in ${dsids[@]}; do
  echo $dsid

  datasets=($(rucio list-dids "mc16_13TeV.$dsid.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ${slice}W.recon.AOD.*" | grep -v "_a" | grep -v _tid | grep CONTAINER | awk '{print $2}'))

  echo $ds >> $dsid.log
  for ds in ${datasets[@]}; do
    echo $ds
    rucio list-dataset-replicas $ds >> $dsid.log 
  done

  let slice=$slice+1

done

