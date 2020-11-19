#pragma once

#include "Modules/ModuleInterface.h"

class ITargetPlatformManagerModule
	: public IModuleInterface
{
public:
	/**
	 * Determine if there were errors during the initialization of the platform manager.
	 *
	 * @param OutErrorMessages Optional pointer to an FString that will have the error messages appended to it.
	 * @return True if there were errors during the initialization of the platform manager, False otherwise.
	 */
	virtual bool HasInitErrors(FString* OutErrorMessages) const = 0;
};