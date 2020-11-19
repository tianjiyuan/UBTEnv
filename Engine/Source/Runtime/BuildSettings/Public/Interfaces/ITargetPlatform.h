#pragma once

#include "CoreMinimal.h"

/**
 * Interface for target platforms.
 *
 * This interface provides an abstraction for cooking platforms and enumerating actual target devices.
 */
class ITargetPlatform
{
public:
	/**
	 * Gets the platform's INI name (so an offline tool can load the INI for the given target platform).
	 *
	 * @see PlatformName
	 */
	virtual FString IniPlatformName() const = 0;

	/**
	 * Gets a new compression format to use in place of Zlib. This should be rarely implemented
	 *
	 * @return Compression format to use instead of Zlib
	 */
	virtual FName GetZlibReplacementFormat() const = 0;

	/**
	 * Checks whether this platform has only editor data (typically desktop platforms).
	 *
	 * @return true if this platform has editor only data, false otherwise.
	 */
	virtual bool HasEditorOnlyData() const = 0;
};