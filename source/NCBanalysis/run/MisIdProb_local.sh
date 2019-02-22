
source InputCollections/set_sample.sh mc16_13TeV.309680.BeamHaloGenerator_BeamGasB1_20MeV.merge.AOD.e6513_e5984_s3262_r10298_r9315 

#----------------------------------------------------------------------------
export InputCollections

dirname=$(dirname "$InputCollections")
dirnameBase=$(basename $dirname)

CWD=$PWD
WorkDir=$STORAGE/NCB/NCBTaggerAnalysis/WorkDir/$(date +%y%m%d.%H%M).$dirnameBase
export WorkDir
echo $WorkDir
mkdir $WorkDir
cp MisIdProb_jobOptions.py $WorkDir
cd $WorkDir
if [ $? -ne 0 ]; then echo "failed to cd to WorkDir"; return; fi

# --nprocs=-1 : use all available cpus
# --nprocs=0  : don't use AthenaMP
export ATHENA_PROC_NUMBER=0
athena -c "from AthenaCommon.AppMgr import theApp; theApp.EvtMax=37" MisIdProb_jobOptions.py

cd $CWD
