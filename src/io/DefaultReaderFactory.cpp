#include "io/DefaultReaderFactory.h"

// phase space files
#include "io/ASCIIReader.h"
#include "io/BinaryReader.h"
#ifdef ENABLE_ADIOS2
#include "io/adios2/Adios2Reader.h"
#endif

// generators
#include "io/CubicGridGeneratorInternal.h"
#include "io/Mkesfera.h"
#include "io/MultiObjectGenerator.h"
#include "io/PerCellGenerator.h"
#include "io/ReplicaGenerator.h"
#include "io/TcTS.h"

DefaultReaderFactory::DefaultReaderFactory() {
	REGISTER_PLUGIN(ASCIIReader);
	REGISTER_PLUGIN(BinaryReader);
#ifdef ENABLE_ADIOS2
	REGISTER_PLUGIN(Adios2Reader);
#endif

	REGISTER_PLUGIN(CubicGridGeneratorInternal);
	REGISTER_PLUGIN(MkesferaGenerator);
	REGISTER_PLUGIN(MkTcTSGenerator);
	REGISTER_PLUGIN(MultiObjectGenerator);
	REGISTER_PLUGIN(PerCellGenerator);
	REGISTER_PLUGIN(ReplicaGenerator);
}
