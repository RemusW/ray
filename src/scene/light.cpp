#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	glm::dvec3 shadAtten;
	glm::dvec3 d = glm::normalize(getDirection(p));				// pointQ -> light
	ray shadRay(p,d,glm::dvec3(1,1,1),ray::SHADOW);				// ray from pointQ to direction of the light
	isect hitT;													// information of first thing hit on the way to the light
	bool inter = scene->intersect(shadRay, hitT);
	if (inter)
		shadAtten = glm::dvec3(0,0,0);
	else
		shadAtten = glm::dvec3(1,1,1);
	return shadAtten;
}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3& P) const
{

	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0
	// isect i;
	// glm::dvec3 cam = scene->getCamera().getEye();
	// ray r(cam, glm::normalize(cam-P), glm::dvec3(1,1,1),ray::VISIBILITY);
	// bool hit = scene->intersect(r, i);
	double d = glm::length(position - P);
	double atten = 1 / (constantTerm + linearTerm*d + quadraticTerm*d*d);
	atten = min(1.0, atten);
	return atten;
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	// r is the ray from camera to pointQ
	glm::dvec3 shadAtten;
	glm::dvec3 d = glm::normalize(getDirection(p));				// pointQ -> light
	ray shadRay(p,d,glm::dvec3(1,1,1),ray::SHADOW);				// ray from pointQ to direction of the light
	isect hitT;													// information of first thing hit on the way to the light
	bool check = scene->intersect(shadRay, hitT);
	double dist2light = glm::length(position - p);
	double dist2Q = glm::length(getDirection(p) * hitT.getT());
	if (dist2light > dist2Q)
		// our surface hit is before the light, apply attenuation
		shadAtten = glm::dvec3(0,0,0);
	else
		// our surface is behind the light, no need to apply attenuation
		shadAtten = glm::dvec3(1,1,1);
	return shadAtten;
}

#define VERBOSE 0

