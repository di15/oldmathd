#ifndef ROAD_H
#define ROAD_H

#include "connectable.h"
#include "resources.h"
#include "../math/vec3i.h"
#include "conduit.h"

class RoadTile : public CdTile
{
public:
	unsigned char cdtype();
	void fillcollider();
	void freecollider();
};

#endif

