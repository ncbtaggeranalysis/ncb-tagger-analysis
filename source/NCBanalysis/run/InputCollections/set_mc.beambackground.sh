samples=(
mc16_13TeV.309680.BeamHaloGenerator_BeamGasB1_20MeV.merge.AOD.e6513_e5984_s3262_r10298_r9315
mc16_13TeV.309681.BeamHaloGenerator_BeamHaloB1_20MeV.merge.AOD.e6513_e5984_s3262_r10298_r9315
mc16_13TeV.309682.BeamHaloGenerator_BeamGasB1_20GeV.merge.AOD.e6513_e5984_s3262_r10298_r9315
mc16_13TeV.309683.BeamHaloGenerator_BeamHaloB1_20GeV.merge.AOD.e6513_e5984_s3262_r10298_r9315
mc16_13TeV.309684.BeamHaloGenerator_BeamGasB1_20GeV_muon.merge.AOD.e6513_e5984_s3262_r10298_r9315
mc16_13TeV.309685.BeamHaloGenerator_BeamHaloB1_20GeV_muon.merge.AOD.e6513_e5984_s3262_r10298_r9315
)

ds=""
for ds in ${samples[@]}; do
  if [[ $ds == *$1* ]]; then break; fi
done

InputCollections="$STORAGE/$ds/*"
