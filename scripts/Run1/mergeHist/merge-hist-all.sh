#!/bin/bash
#
# runs with : bash merge-hist-all.sh 
#

DIR=../efficiency/HIST/user.olariu.data11_7TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz/
#DIR=../efficiency/HIST/user.olariu.data12_8TeV.physics_Background.NTUP_BKGD.grl.190104.1741_XYZ.HIST.root.tgz

#DIR=../misIdProb/HIST/user.olariu.data11_7TeV.physics_JetTauEtmiss.test.181229.0819_XYZ.HIST.root.tgz/

if [ ! -d $DIR/merge-all ]; then mkdir $DIR/merge-all; fi 

if [[ $DIR == *data11_7TeV* ]]; then project_tag=data11_7TeV
elif [[ $DIR == *data12_8TeV* ]]; then project_tag=data12_8TeV
else echo "unknown project tag"; exit; fi

hadd -f $DIR/merge-all/$project_tag.merge.HIST.root $(find $DIR -type f -name "*.HIST.root" | grep dir)


