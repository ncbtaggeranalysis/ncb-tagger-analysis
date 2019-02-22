setupATLAS

cd build
#asetup AtlasProduction,21.0.20.1,here
asetup Athena,21.0.82,here
#asetup AtlasProduction,21.9.0,here
mv CMakeLists.txt ../source

cmake ../source
make
#source x86_64-slc6-gcc62-opt/setup.sh 
source $TestArea/*/setup.sh
cd .. 

localSetupPandaClient
localSetupRucioClients
voms-proxy-init -voms atlas
