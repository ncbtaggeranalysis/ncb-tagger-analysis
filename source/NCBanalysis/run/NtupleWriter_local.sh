#source InputCollections/set_mc.beambackground.sh 309680
#source InputCollections/set_mc.satellite.sh AtlasG4_tf.py 30000
source InputCollections/set_sample.sh data18_13TeV.00348885.physics_Background.merge.DAOD_IDNCB.f921_m1950

#--------------------------------------------------------------------
export InputCollections

dirname=$(dirname "$InputCollections")
dirnameBase=$(basename $dirname)

CWD=$PWD
WorkDir=$STORAGE/NCB/NCBTaggerAnalysis/WorkDir/$(date +%y%m%d.%H%M).$dirnameBase
export WorkDir
echo $WorkDir
mkdir $WorkDir
cp NtupleWriter_jobOptions.py $WorkDir
cd $WorkDir
if [ $? -ne 0 ]; then echo "failed to cd to WorkDir"; return; fi

# --nprocs=-1 : use all available cpus
# --nprocs=0  : don't use AthenaMP
export ATHENA_PROC_NUMBER=0
athena -c "from AthenaCommon.AppMgr import theApp; theApp.EvtMax=1000" NtupleWriter_jobOptions.py # >> /dev/null &

cd $CWD
