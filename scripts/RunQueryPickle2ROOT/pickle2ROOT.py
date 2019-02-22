import sys
import pickle
from ROOT import TFile, TTree, std
import numpy

def str2bool(value):
  if value=='FALSE':
    return False
  elif value=='TRUE':
    return True
  else:
    raise 'Error: str2bool wrong value.'
  
if len(sys.argv) != 2:
  print "Syntax is: ",sys.argv[0], "<pickle file>"
  exit

try:
  fpickle=open(sys.argv[1],"rb")
except:
  print "failed to open pickle file"
  exit

dict=pickle.load(fpickle)

if len(dict['Run'])!=1:
  print 'Error: pickle dictionary contains more than just one run.'
  exit

rootfile=str(dict['Run'][0])+'.root'
print '==>', rootfile

if str(dict['Run'][0]) not in sys.argv[1]:
  print 'Error: run numbers from the pickle filename and pickle dictionary mismatch.'
  exit

froot = TFile(rootfile,'recreate')
tree = TTree('CollectionTree','')

runNumber=numpy.zeros(1, dtype=numpy.uint32)
runNumber[0]=dict['Run'][0]
tree.Branch('RunNumber',runNumber,'RunNumber/i')

stablebeams_firstlb=std.vector('int')()
stablebeams_lastlb=std.vector('int')()
stablebeams_value=std.vector('bool')()
tree.Branch('stablebeams_firstlb','vector<int>',stablebeams_firstlb)
tree.Branch('stablebeams_lastlb','vector<int>',stablebeams_lastlb)
tree.Branch('stablebeams_value','vector<bool>',stablebeams_value)

for i in range(len(dict[runNumber[0]]['lhc:stablebeams'])):
  stablebeams_firstlb.push_back(dict[runNumber[0]]['lhc:stablebeams'][i]['firstlb'])
  stablebeams_lastlb.push_back(dict[runNumber[0]]['lhc:stablebeams'][i]['lastlb'])
  stablebeams_value.push_back(str2bool(dict[runNumber[0]]['lhc:stablebeams'][i]['value']))

beam1intensity_firstlb=std.vector('int')()
beam1intensity_lastlb=std.vector('int')()
beam1intensity_value=std.vector('float')()
beam2intensity_firstlb=std.vector('int')()
beam2intensity_lastlb=std.vector('int')()
beam2intensity_value=std.vector('float')()

tree.Branch('beam1intensity_firstlb','vector<int>',beam1intensity_firstlb)
tree.Branch('beam1intensity_lastlb','vector<int>',beam1intensity_lastlb)
tree.Branch('beam1intensity_value','vector<float>',beam1intensity_value)
tree.Branch('beam2intensity_firstlb','vector<int>',beam2intensity_firstlb)
tree.Branch('beam2intensity_lastlb','vector<int>',beam2intensity_lastlb)
tree.Branch('beam2intensity_value','vector<float>',beam2intensity_value)

for i in range(len(dict[runNumber[0]]['olc:beam1intensity'])):
  beam1intensity_firstlb.push_back(dict[runNumber[0]]['olc:beam1intensity'][i]['firstlb'])
  beam1intensity_lastlb.push_back(dict[runNumber[0]]['olc:beam1intensity'][i]['lastlb'])
  beam1intensity_value.push_back(float(dict[runNumber[0]]['olc:beam1intensity'][i]['value'])) 

for i in range(len(dict[runNumber[0]]['olc:beam2intensity'])):
  beam1intensity_firstlb.push_back(dict[runNumber[0]]['olc:beam2intensity'][i]['firstlb'])
  beam1intensity_lastlb.push_back(dict[runNumber[0]]['olc:beam2intensity'][i]['lastlb'])
  beam1intensity_value.push_back(float(dict[runNumber[0]]['olc:beam2intensity'][i]['value']))

tree.Fill()
tree.Write()
froot.Close()
