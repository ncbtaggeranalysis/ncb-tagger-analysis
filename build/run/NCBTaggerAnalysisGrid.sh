#CMD="--check-ds-replicas:CERN-PROD_SCRATCHDISK"
#CMD="--check-ds-replicas:DISK"
#CMD="--check-ds-size"
CMD="--check-ds-num-events"
#CMD="--check-tool-input-files"
#CMD="--rucio-add-rule:CERN-PROD_SCRATCHDISK"
#CMD="--grid-send-ntuplewriter"
#CMD="--grid-send-misid-prob"
#CMD="--inputFileList-create-with-totalsize-per-DS-limit"
#CMD="--inputFileList-create-replicas-on-disk:UKI-SCOTGRID-GLASGOW_SCRATCHDISK"

# -- data --------------------------------------------------------------------
#DS_DICTIONARY=ds_dict_physics_Background_DAOD_IDNCB.py
#DS_DICTIONARY=ds_dict_physics_Main_AOD.py
#DS_DICTIONARY=ds_dict_physics_Main_DAOD_EXOT2.py

#PROJECT_TAG=data15_13TeV
#PERIOD_LIST=periodC
#RUN_NUMBER_LIST=271298,271421

# -- MC ----------------------------------------------------------------------
DS_DICTIONARY=ds_dict_mc.py
PROJECT_TAG=mc16_13TeV

#PERIOD_LIST=beam-background
#RUN_NUMBER_LIST=309684,309685

PERIOD_LIST=jet-jet
#RUN_NUMBER_LIST=
# ---------------------------------------------------------------------------- 

GRID_JOB_EXTRA_OPTS=""
#GRID_JOB_EXTRA_OPTS="--nFiles=1"
#GRID_JOB_EXTRA_OPTS="--site=ANALY_CERN"
#GRID_JOB_EXTRA_OPTS="--nJobs=10,--nEventsPerJob=20000"
#GRID_JOB_EXTRA_OPTS="--nJobs=10,--nEventsPerJob=100000"

GRID_SEND_OPTION="onejob-per-dataset-with-tarball"

#-----------------------------------------------------------------------------

python NCBTaggerAnalysisGrid.py \
--projectTag:$PROJECT_TAG \
--period-list:$PERIOD_LIST \
--runNumber-list:$RUN_NUMBER_LIST \
--ds-dictionary:$DS_DICTIONARY \
--grid-job-extra-opts:$GRID_JOB_EXTRA_OPTS \
--grid-send-option:$GRID_SEND_OPTION \
$CMD 
