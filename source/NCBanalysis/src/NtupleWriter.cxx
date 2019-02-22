#include "NCBanalysis/NtupleWriter.h"

NtupleWriter::NtupleWriter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator ),
  m_ntupleWriterTool("NtupleWriterTool/NtupleWriterTool") {
  
}

NtupleWriter::~NtupleWriter() {
}

StatusCode NtupleWriter::initialize() {

   return StatusCode::SUCCESS;
 }

StatusCode NtupleWriter::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode NtupleWriter::execute() {
  
  ATH_CHECK(m_ntupleWriterTool->Fill());

  return StatusCode::SUCCESS;
}

