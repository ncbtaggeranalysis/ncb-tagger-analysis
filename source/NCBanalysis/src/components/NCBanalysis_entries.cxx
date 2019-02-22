#include "GaudiKernel/DeclareFactoryEntries.h"
#include "NCBanalysis/NtupleWriter.h"
#include "NCBanalysis/MisIdProb.h"
#include "NCBanalysis/BeamBackgroundFiller.h"
#include "NCBanalysis/BunchStructureTool.h"
#include "NCBanalysis/BeamIntensityTool.h"
#include "NCBanalysis/HistTool.h"
#include "NCBanalysis/NtupleWriterTool.h"
#include "NCBanalysis/RunQueryPickleTool.h"

DECLARE_ALGORITHM_FACTORY( NtupleWriter )
DECLARE_ALGORITHM_FACTORY( MisIdProb )
DECLARE_ALGORITHM_FACTORY( BeamBackgroundFiller )
DECLARE_TOOL_FACTORY( BunchStructureTool )
DECLARE_TOOL_FACTORY( BeamIntensityTool )
DECLARE_TOOL_FACTORY( HistTool )
DECLARE_TOOL_FACTORY( NtupleWriterTool )
DECLARE_TOOL_FACTORY( RunQueryPickleTool )

DECLARE_FACTORY_ENTRIES ( NCBanalysis ) {
  DECLARE_ALGORITHM ( NtupleWriter )
  DECLARE_ALGORITHM ( MisIdProb )
  DECLARE_ALGORITHM ( BeamBackgroundFiller )
  DECLARE_TOOL( BunchStructureTool )
  DECLARE_TOOL( BeamIntensityTool )
  DECLARE_TOOL( HistTool )
  DECLARE_TOOL( NtupleWriterTool )
  DECLARE_TOOL( RunQueryPickleTool )
}
