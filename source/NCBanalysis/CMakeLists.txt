# Declare the package name:
atlas_subdir( NCBanalysis )

atlas_depends_on_subdirs(
  PUBLIC
  GaudiKernel
  PRIVATE
  Control/AthToolSupport/AsgTools
  Calorimeter/CaloGeoHelpers 
  Control/xAODRootAccess 
#  #Control/AthLinks
  DataQuality/GoodRunsLists
  Event/xAOD/xAODEventInfo 
  Event/xAOD/xAODJet 
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODMuon 
  Event/xAOD/xAODEgamma 
  Event/xAOD/xAODTracking 
  Event/xAOD/xAODMissingET 
  Event/xAOD/xAODTrigger
  Event/xAOD/xAODTracking
  Event/xAOD/xAODTruth
  Generators/GeneratorObjects
  InnerDetector/InDetRawEvent/InDetBCM_RawData
  InnerDetector/InDetDetDescr/InDetIdentifier
  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
  MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
  PhysicsAnalysis/JetMissingEtID/JetSelectorTools 
  PhysicsAnalysis/MuonID/MuonSelectorTools 
  PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
  Reconstruction/Jet/JetCalibTools
  Tools/PathResolver
  TileCalorimeter/TileEvent
  Trigger/TrigAnalysis/TrigDecisionTool
  # packages for reading the HITS
  DetectorDescription/GeoModel/GeoAdaptors
  InnerDetector/InDetDetDescr/InDetIdentifier
  InnerDetector/InDetSimEvent
  LArCalorimeter/LArSimEvent
  TileCalorimeter/TileSimEvent
  Calorimeter/CaloSimEvent
  MuonSepectrometer/MuonSimEvent
)

find_package( ROOT COMPONENTS MathCore RIO Core Tree Hist )

# Libraries in the package:
atlas_add_library( NCBanalysisLib
  src/*.cxx NCBanalysis/*.h 
  #src/components/*.cxx
  PUBLIC_HEADERS NCBanalysis
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
  LINK_LIBRARIES ${ROOT_LIBRARIES}
  GaudiKernel
  AsgTools xAODRootAccess
  GeneratorObjects
  GoodRunsListsLib
  CaloGeoHelpers
  InDetBCM_RawData InDetIdentifier
  xAODEventInfo xAODJet xAODCaloEvent xAODMuon xAODEgamma xAODTracking xAODMuon xAODMissingET xAODTrigger xAODTracking xAODTruth
  PathResolver
  MuonSelectorToolsLib MuonMomentumCorrectionsLib MuonPrepRawData MuonPattern
MuonSegment MuonRecHelperToolsLib 
  JetSelectorToolsLib JetCalibToolsLib 
  TileEvent 
  TrigDecisionToolLib 
  GeoAdaptors InDetSimEvent LArSimEvent TileSimEvent CaloSimEvent MuonSimEvent  
)

atlas_add_component( NCBanalysis src/components/*.cxx
  LINK_LIBRARIES NCBanalysisLib
  GaudiKernel
  AsgTools xAODRootAccess
  GeneratorObjects
  GoodRunsListsLib
  CaloGeoHelpers
  InDetBCM_RawData InDetIdentifier
  xAODEventInfo xAODJet xAODCaloEvent xAODMuon xAODEgamma xAODTracking xAODMuon xAODMissingET xAODTrigger xAODTracking xAODTruth
  PathResolver
  JetSelectorToolsLib MuonSelectorToolsLib MuonMomentumCorrectionsLib JetCalibToolsLib 
  TileEvent 
  TrigDecisionToolLib 
  MuonPrepRawData MuonPattern MuonSegment
  GeoAdaptors InDetSimEvent LArSimEvent TileSimEvent CaloSimEvent MuonSimEvent 
)

atlas_install_data( data/* )
atlas_install_python_modules( python/* )
atlas_install_xmls( data/*)
