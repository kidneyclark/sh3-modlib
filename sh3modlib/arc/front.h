#ifndef ARC_FRONT_H
#define ARC_FRONT_H

#include <com/defs.h>

#include <mem/types.h>
#include "types.h"

COM_CONTEXT_DECL(arc)

// Mandatory. 
// <path> should be a path to the folder containing 'arc.arc'.
void arc_SetDataDirectory(const char *path);

// Mandatory. 
// Using the 'arc.arc' data, it builds the internal structure.
// Loads from disk.
void arc_LoadData();
bool arc_IsDataLoaded();
void arc_FreeData();

// It will cache the array and return that if
// no changes are made.
arc_Cluster arc_GetCluster(const char *name);
mem_Array<arc_Cluster> arc_GetClusters();

/* For later...
	size_t arc_GetClusterIndex(const char *cluster);
	size_t arc_GetClusterCount();
	// Number of vfiles inside the cluster
	size_t arc_GetClusterVfilesCount(size_t idx);
	const char *arc_GetClusterName(size_t idx);
*/

// It will cache the array and return that if
// no changes are made.
arc_Vfile arc_GetVfile(const char *name);
mem_Array<arc_Vfile> arc_GetVfiles();

/* For later...
	size_t arc_GetVfileIndex(const char *vfile);
	size_t arc_GetVfileCount();
	const char *arc_GetVfileName(size_t idx);
	// Gets which cluster stores this vfile
	size_t arc_GetVfileCluster(size_t idx);
	// IMPORTANT: This is the vfile index INSIDE the cluster!
	size_t arc_GetVfileIndex(size_t idx);
*/

// Must be freed with arc_FreeVfile(mem_Chunk)!
// Loads from disk.
mem_Chunk arc_LoadVfile(const char *vfile);
void arc_FreeVfile(mem_Chunk chunk);

#endif