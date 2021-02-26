#include "cubeMap.h"
#include "ray.h"
#include "../ui/TraceUI.h"
#include "../scene/material.h"

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <ostream>
using namespace std;
extern TraceUI* traceUI;


glm::dvec3 CubeMap::getColor(ray r) const
{
	// YOUR CODE HERE
	// FIXME: Implement Cube Map here
	// do phong model?
	glm::dvec3 dir = r.getDirection();
	dir[0] = abs(dir[0]);
	dir[1] = abs(dir[1]);
	dir[2] = abs(dir[2]);
	int greatest = 0;
	double maxs = 0;
	maxs = max(dir[0], dir[1]);
	maxs==dir[0]? greatest=0 : greatest = 1;
	maxs = max(dir[greatest], dir[2]);
	maxs==dir[2]? greatest=2 : greatest = greatest;
	
	dir = r.getDirection();
	dir[greatest] = abs(dir[greatest]);
	glm::dvec2 coord(0,0);
	if(greatest == 0){ // X-axis
		coord = glm::dvec2(dir[1], dir[2]);
		coord[0] = 0.5 * (dir[2] / dir[greatest] + 1);
		coord[1] = 0.5 * (dir[1] / dir[greatest] + 1);
		return r.getDirection()[greatest]>0 ? tMap[0]->getMappedValue(coord) : tMap[1]->getMappedValue(coord);
	}
	else if(greatest == 1) { // Y-axis
		coord = glm::dvec2(dir[0], dir[2]);
		coord[0] = 0.5 * (dir[0] / dir[greatest] + 1);
		coord[1] = 0.5 * (dir[2] / dir[greatest] + 1);
		return r.getDirection()[greatest]>0 ? tMap[2]->getMappedValue(coord) : tMap[3]->getMappedValue(coord);
	}
	else if(greatest == 2) { // Z-axis
		coord[0] = 0.5 * (dir[0] / dir[greatest] + 1);
		coord[1] = 0.5 * (dir[1] / dir[greatest] + 1);
		return r.getDirection()[greatest]>0 ? tMap[4]->getMappedValue(coord) : tMap[5]->getMappedValue(coord);
	}
	return r.at(1.0);
}

CubeMap::CubeMap()
{
}

CubeMap::~CubeMap()
{
}

void CubeMap::setNthMap(int n, TextureMap* m)
{
	if (m != tMap[n].get())
		tMap[n].reset(m);
}
