DATA_DIR=$STORAGE/NCB/NCBTaggerAnalysis
#DATA_DIR=$NCB/data

VERSION=190211

# data 
#PROJECT_TAG=data16_13TeV
#PROJECT_TAG=data17_13TeV
#PROJECT_TAG=data18_13TeV
#PROJECT_TAG=dataRun2_13TeV

#STREAM=physics_Background
#STREAM=physics_Main

# mc
PROJECT_TAG=mc16_13TeV
#STREAM=beambackground-mc
STREAM=jetjet

#-------------------------------------- -----------------------------------------------

if [ ! -d $VERSION ]; then mkdir $VERSION; fi

txtfile=$VERSION/$PROJECT_TAG.$STREAM.txt
if [ -f $txtfile ]; then rm $txtfile; fi
touch $txtfile
find $DATA_DIR/$VERSION -name "*NtupleWriter.merge.root" | sort > $txtfile
echo $txtfile
echo "$(cat $txtfile | wc -l) runs"
