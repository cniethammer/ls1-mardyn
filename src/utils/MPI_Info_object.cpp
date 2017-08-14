#include "utils/MPI_Info_object.h"

#include "Simulation.h"
#include "utils/Logger.h"

#if ENABLE_MPI

MPI_Info_object::~MPI_Info_object() {
	if(_mpi_info!=MPI_INFO_NULL) {
		MPI_CHECK( MPI_Info_free(&_mpi_info) );
		//_mpi_info=MPI_INFO_NULL;
	}
}

void MPI_Info_object::readXML(XMLfile& xmlconfig) {
	if(_mpi_info==MPI_INFO_NULL)
		MPI_CHECK( MPI_Info_create(&_mpi_info) );
	
	XMLfile::Query query = xmlconfig.query("hint");
	global_log->debug() << "Number of hint key value pairs: " << query.card() << endl;
	
	string oldpath = xmlconfig.getcurrentnodepath();
	for(auto pairIter = query.begin(); pairIter; ++pairIter) {
		xmlconfig.changecurrentnode(pairIter);
		std::string key, value;
		xmlconfig.getNodeValue("key", key);
		xmlconfig.getNodeValue("value", value);
		global_log->debug() << "Found MPI Info hint '" << key << "': " << value << std::endl;
		add_hint(key, value);
	}
	xmlconfig.changecurrentnode(oldpath);
}

void MPI_Info_object::add_hint(std::string& key, std::string& value) {
	if(key.size() > MPI_MAX_INFO_KEY) {
		global_log->error() << "MPI Info key name longer than allowed." << std::endl;
		return;
	}
	if(value.size() > MPI_MAX_INFO_VAL) {
		global_log->error() << "MPI Info value longer than allowed." << std::endl;
		return;
	}
	MPI_CHECK( MPI_Info_set(_mpi_info, key.c_str(), value.c_str()) );
}

void MPI_Info_object::get_dup_MPI_Info(MPI_Info* mpi_info) {
	MPI_CHECK( MPI_Info_dup(_mpi_info, mpi_info) );
}

#endif  // ENABLE_MPI
