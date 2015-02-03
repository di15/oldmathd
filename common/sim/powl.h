

#ifndef POWL_H
#define POWL_H

#include "connectable.h"
#include "resources.h"
#include "../render/vertexarray.h"
#include "../math/vec3i.h"
#include "conduit.h"

class PowlTile : public ConduitTile
{
public:
	char condtype();
};

#endif

