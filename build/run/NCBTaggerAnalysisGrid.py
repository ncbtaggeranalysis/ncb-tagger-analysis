import sys
import os
import subprocess
from subprocess import Popen, PIPE

def check_ds_replicas(ds_list, storage_element):
  print 'Checking dataset replicas on ', storage_element
  all_ds_replicated=True
  for ds in ds_list:
    print ds
      
    p = Popen(['rucio', 'list-dataset-replicas', ds], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate("input data that is passed to subprocess' stdin")
    rc = p.returncode

    print output

    isReplicated=False
    for line in output.splitlines():
      if storage_element in line:
        isReplicated=True
        found=line.split()[3]
        total=line.split()[5]
        if found != total:
          print 'Replication not yet finished: found slots=',found,' total=',total
          isReplicated=False
        else:
          break

    if not isReplicated:
      print 'Dataset is not replicated to ',storage_element
      all_ds_replicated=False

  if all_ds_replicated:
    print "All datasets replicated to ", storage_element
  else:
    raise NameError('Not all datasets are replicated to ', storage_element)

def check_tool_input_files(ds_list):
  print 'Checking tool input files'
  for ds in ds_list:
    print ds
    runNumStr=ds.split('.')[1]
    runNumStr=str(int(runNumStr)) # remove 00 in front
    sourceDir='../../source'

    if not os.path.isfile(sourceDir+"/NCBanalysis/data/BunchStructureTool/"+ runNumStr + ".txt" ):
      raise NameError('BunchStructureTool txt file missing for run ', runNumStr)

    if not os.path.isfile(sourceDir+"/NCBanalysis/data/BeamIntensityTool/" + runNumStr + ".txt" ):
      raise NameError('BeamIntensityTool txt file missing for run ', runNumStr)

    if not os.path.isfile(sourceDir+"/NCBanalysis/data/RunQueryPickleTool/" + runNumStr + ".root"):
      raise NameError('RunQueryPickleTool file missing for run ', runNumStr)

  print "All input files available."

def rucio_add_rule(ds_list, storage_element):

    if os.environ['RUCIO_HOME'] == '':
      print 'Please setup rucio.'
      return

    for ds in ds_list:
      cmd='rucio add-rule --notify N ' + ds + ' 1 ' + storage_element
      print cmd
      subprocess.call(cmd, shell=True)

def grid_send(option, date_tag, ds_dictionary, project_tag, ds_list, jobOptions, extOutFile, extraOpts):

  if '_mc' not in ds_dictionary:
    check_tool_input_files(ds_list)
  check_ds_replicas(ds_list, 'DISK')
  check_ds_num_events(ds_list)

  user=subprocess.check_output('whoami', shell=True).rstrip('\n')

  #os.chdir('source')

  if option == 'onejob-inDsTxt': 
    name=ds_dictionary
    name = name.replace('ds_dict_','')
    name = name.replace('.py','')
    outDS='user.' + user + '.' + name + '.' + date_tag

    f = open("inDs.txt","w") 
    for ds in ds_list:
      f.write(ds+'\n')
    f.close()   
 
    cmd='pathena '+jobOptions+' --useContElementBoundary --addNthFieldOfInDSToLFN=1,2,3,5 --inDsTxt inDs.txt --outDS ' + outDS + ' --extOutFile '+extOutFile + ' --tmpDir /tmp ' + extraOpts
    
    print cmd
    subprocess.call(cmd, shell=True)

  elif option == 'onejob-inputFileList':
    for ds in ds_list: 
      outDS='user.' + user + '.' + ds + "." + date_tag
      cmd='pathena '+jobOptions+' --inDS '+ds+' --inputFileList=inputFileList.txt --outDS=' + outDS + ' --extOutFile='+extOutFile + ' --tmpDir /tmp/' + ' ' + extraOpts
      print cmd
      subprocess.call(cmd, shell=True)


  elif option == 'onejob-per-dataset':
    for ds in ds_list:
      outDS='user.' + user + '.' + ds + "." + date_tag
      cmd='pathena '+jobOptions+' --inDS=' + ds + ' --outDS=' + outDS + ' --extOutFile='+extOutFile + ' --tmpDir /tmp/' + ' ' + extraOpts
      print cmd
      subprocess.call(cmd, shell=True)

  elif option == 'onejob-per-dataset-with-tarball':   
    cmd='pathena --tmpDir /tmp --noSubmit --outTarBall /tmp/tarBall.tgz --outDS user.'+user+'.dummy ' + jobOptions
    print cmd
    subprocess.call(cmd, shell=True) 
    for ds in ds_list:
      outDS='user.' + user + '.' + ds.split('.')[0] + '.' + ds.split('.')[1] + '.' + ds.split('.')[2] + '.' + ds.split('.')[4] + "." + date_tag
      cmd='pathena '+jobOptions+' --inTarBall /tmp/tarBall.tgz --inDS=' + ds + ' --outDS=' + outDS + ' --extOutFile='+extOutFile + ' --tmpDir /tmp/' + ' ' + extraOpts
      print cmd
      subprocess.call(cmd, shell=True)  

def check_size(ds_list):
    total_size=0
    for ds in ds_list:
      p = Popen(['rucio','list-files',ds], stdin=PIPE, stdout=PIPE, stderr=PIPE)
      output, err = p.communicate("input data that is passed to subprocess' stdin")
      rc = p.returncode
      for line in output.splitlines():
        if 'Total size :' in line:
          #print line
          sizeStr=line.split(":")[1]
          #print "sizeStr=",sizeStr.split()
          size=float(sizeStr.split()[0])
          unit=sizeStr.split()[1]
          if unit == "MB":
            size=size/1000
          elif unit == "TB":
            size=size*1000
          elif unit != "GB":
            raise NameError("Unknown unit.")

          print ds, "%8.0f" % size,"GB"
          total_size=total_size+size

    print "Total=", "%8.0f" % total_size, "GB"

def check_ds_num_events(ds_list):
  print 'Checking the number of events in dataset'
  total_events=0
  for ds in ds_list:
    p = Popen(['rucio','list-files', ds], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate("input data that is passed to subprocess' stdin")
    rc = p.returncode
    events=0
    for line in output.splitlines():
      if 'data' not in line and  'mc' not in line:
        continue
      events+=int(line.split('|')[5])
    print ds,' ', '%10i' % events
    total_events+=events
  print 'Total events: ',total_events

#def inputFileListCreateWithTotalSizePerDSLimit(ds_list, projectTag, period_list, runNumber): 
#  print 'Create inDs txt file with total size pe DS limit ...'
#  filename='inDs.'+projectTag+'.';
#  if runNumber != '':
#    filename=filename+runNumber+'.';
#  else:
#    for period in period_list:
#      filename=filename+period+'.';
#  filename=filename+'txt'
#  print 'filename=',filename
#  f = open(filename,'w')
#  os.chdir('scripts/inputFileListForPathena')

#  for ds in ds_list:
#    p = Popen(['zsh','inputFileList.sh',ds,'GRL_OFF','50GB'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
#    output, err = p.communicate("input data that is passed to subprocess' stdin")
#    rc = p.returncode
#    for line in output.splitlines(): 
#      print line
#      if '==>' in line:
#        f.write(line.split('==> ')[1]+'\n') 
#  f.close()
#  os.chdir('../..')

def inputFileListCreateReplicasOnDisk(inDsTxtFile, disk):
  print 'inputFileListCreateReplicasOnDisk: ',disk
  f = open(inDsTxtFile)
  for filename in f:
    filename=filename.rstrip('\n')
    print filename
    cmd='rucio add-rule --notify N ' + filename + ' 1 ' + disk
    print cmd
    subprocess.call(cmd, shell=True)
  f.close()

projectTag=''
runNumber_list=''
grid_job_site=''
grid_job_extraOpts=''
grid_send_option=''

for i in range(1,len(sys.argv)):
  if '--projectTag' in sys.argv[i]:
    projectTag=sys.argv[i].split(':')[1]
  if '--period-list' in sys.argv[i]:
    period_list=sys.argv[i].split(':')[1]
  if '--runNumber-list' in sys.argv[i]:
    runNumber_list=sys.argv[i].split(':')[1].split(',')
  if '--ds-dictionary' in sys.argv[i]:
    ds_dictionary=sys.argv[i].split(':')[1]
  if '--grid-job-extra-opts' in sys.argv[i]:
    grid_job_extraOpts=sys.argv[i].split(':')[1]
    grid_job_extraOpts=grid_job_extraOpts.replace(',',' ') 
  if '--grid-send-option' in sys.argv[i]:
    grid_send_option=sys.argv[i].split(':')[1]

print 'ds_dictionary=',ds_dictionary
if ds_dictionary == 'ds_dict_physics_Background_DAOD_IDNCB.py': 
  from ds_dict_physics_Background_DAOD_IDNCB import DS_DICT
elif ds_dictionary == 'ds_dict_physics_Main_AOD.py':
  from ds_dict_physics_Main_AOD import DS_DICT 
elif ds_dictionary == 'ds_dict_physics_Main_DAOD_EXOT2.py':
  from ds_dict_physics_Main_DAOD_EXOT2 import DS_DICT
elif ds_dictionary == 'ds_dict_mc.py':
  from ds_dict_mc import DS_DICT
else:
  raise 'Unknown dataset dictionary file.'

ds_list=[]

if period_list == '':
  period_list = DS_DICT[projectTag].keys()
else:
  period_list = period_list.split(',')

for period in period_list:
  for ds in DS_DICT[projectTag][period]:
    if runNumber_list == '':
      ds_list.append(ds)
    else:
      for runNumber in runNumber_list:
        if runNumber in ds:
          ds_list.append(ds)
ds_list.sort()

print "projectTag=",projectTag
print "period_list=",period_list
print "runNumber_list=",runNumber_list
print "ds_list=",ds_list

date_tag=subprocess.check_output('date +"%y%m%d.%H%M"', shell=True).rstrip('\n')

for i in range(1,len(sys.argv)):
  if sys.argv[i] == '--check-tool-input-files':
    check_tool_input_files(ds_list)

  if '--check-ds-replicas' in sys.argv[i]:
    check_ds_replicas(ds_list, sys.argv[i].split(':')[1])

  if sys.argv[i] == '--check-ds-size':
    check_size(ds_list)

  if sys.argv[i] == '--check-ds-num-events':
    check_ds_num_events(ds_list)

  if '--rucio-add-rule' in sys.argv[i]:
    rucio_add_rule(ds_list, sys.argv[i].split(':')[1])

  if sys.argv[i] == '--grid-send-misid-prob':
    grid_send(grid_send_option, date_tag, ds_dictionary, projectTag, ds_list, jobOptions='MisIdProb_jobOptions_grid.py', extOutFile='\"*MisIdProb*.root*\"', extraOpts=grid_job_extraOpts)

  if sys.argv[i] == '--grid-send-ntuplewriter':
    grid_send(grid_send_option, date_tag, ds_dictionary, projectTag, ds_list, jobOptions='NtupleWriter_jobOptions_grid.py', extOutFile='\"*NtupleWriter*.root*\"', extraOpts=grid_job_extraOpts)
  
  #if sys.argv[i] == '--inputFileList-create-with-totalsize-per-DS-limit':
  #  inputFileListCreateWithTotalSizePerDSLimit(ds_list, projectTag, period_list, runNumber)

  if '--inputFileList-create-replicas-on-disk' in sys.argv[i]:
    inputFileListCreateReplicasOnDisk(sys.argv[i].split(':')[1], sys.argv[i].split(':')[2]) 
