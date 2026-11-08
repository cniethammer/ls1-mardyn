#ifndef SRC_IO_DEFAULTREADERFACTORY_H_
#define SRC_IO_DEFAULTREADERFACTORY_H_

#include "io/InputBase.h"
#include "plugins/PluginFactory.h"

/** @brief Factory for all phase space readers and generators (InputBase) available in this build.
 *
 * The constructor registers all readers listed in DefaultReaderFactory.cpp. The set of registered
 * readers depends on enabled CMake options (ADIOS2).
 */
class DefaultReaderFactory : public PluginFactory<InputBase> {
public:
	DefaultReaderFactory();
};

#endif  // SRC_IO_DEFAULTREADERFACTORY_H_
