#pragma once

#include "Modules/ModuleInterface.h"

class FDerivedDataCacheInterface
{

};

/**
 * Module for the DDC
 */
class IDerivedDataCacheModule : public IModuleInterface
{
public:
	/** Return the DDC interface **/
	virtual FDerivedDataCacheInterface& GetDDC() = 0;
};
