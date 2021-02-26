#include "material.h"
#include "../ui/TraceUI.h"
#include "light.h"
#include "ray.h"
extern TraceUI* traceUI;

#include <glm/gtx/io.hpp>
#include <iostream>
#include "../fileio/images.h"

using namespace std;
extern bool debugMode;

Material::~Material()
{
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene* scene, const ray& r, const isect& i) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and
	// shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.
	//	if( debugMode )
	//		std::cout << "Debugging Phong code..." << std::endl;

	// When you're iterating through the lights,
	// you'll want to use code that looks something
	// like this:
	//
	// for ( const auto& pLight : scene->getAllLights() )
	// {
	//              // pLight has type unique_ptr<Light>
	// 		.
	// 		.
	// 		.
	// }
	const Material& m = i.getMaterial();

	glm::dvec3 phong(0,0,0);
	glm::dvec3 pointQ = (r.getPosition()) + glm::normalize(r.getDirection()) *i.getT();
	phong += m.ke(i) + m.ka(i) * scene->ambient();
	for ( const auto& pLight : scene->getAllLights() ) {
		// Calc attenuation
		double distatten = pLight->distanceAttenuation(pointQ);
		glm::dvec3 shadatten = pLight->shadowAttenuation(r, pointQ+pLight->getDirection(pointQ)*RAY_EPSILON);
		glm::dvec3 atten = distatten * shadatten;
		// Calc diffuse
		glm::dvec3 diffuse = kd(i) * max(0.0, dot(i.getN(), pLight->getDirection(pointQ)));
		// Calc specular
		glm::dvec3 R = glm::reflect(pLight->getDirection(pointQ)*-1.0, i.getN());
		glm::dvec3 V = r.getDirection()*-1.0;
		glm::dvec3 specular = ks(i) * pow(max(0.0, dot(R, V)), m.shininess(i));
		phong += atten*(diffuse + specular) * pLight->getColor();
	}
	return phong;
}

TextureMap::TextureMap(string filename)
{
	data = readImage(filename.c_str(), width, height);
	if (data.empty()) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("'.");
		throw TextureMapException(error);
	}
}

glm::dvec3 TextureMap::getMappedValue(const glm::dvec2& coord) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.
	double x = coord[0]*width;
	double y = coord[1]*height;
	int x1 = coord[0]*width;
	int x2 = coord[0]*width+1;
	x2 = glm::clamp(x2,0,width-1);
	int y1 = coord[1]*height;
	int y2 = coord[1]*height+1;
	y2 = glm::clamp(y2,0,height-1);
	glm::dvec3 pixQ11 = getPixelAt(x, y);
	glm::dvec3 pixQ12 = getPixelAt(x1, y2);
	glm::dvec3 pixQ21 = getPixelAt(x2, y1);
	glm::dvec3 pixQ22 = getPixelAt(x2, y2);
	glm::dvec3 fxy1;
	glm::dvec3 fxy2;
	if (x1!=x2) {
		fxy1 = (x2-x)/(x2-x1) * pixQ11 + (x-x1)/(x2-x1) * pixQ21;
		fxy2 = (x2-x)/(x2-x1) * pixQ12 + (x-x1)/(x2-x1) * pixQ22;
	}
	else{
		fxy1 = pixQ11;
		fxy2 = pixQ12;
	}
	glm::dvec3 bilerp;
	if (y1!=y2)
		bilerp = (y2-y)/(y2-y1) * fxy1 + (y-y1)/(y2-y1) * fxy2;
	else
		x1<=x2? bilerp = fxy1 : bilerp = fxy2;
	//glm::dvec3 bilerp = 1.0/((x2-x1)*(y2-y1)) * (pixQ11*(x2-x)*(y2-y) + pixQ21*(x-x1)*(y2-y) + pixQ12*(x2-x)*(y-y1) + pixQ22*(x-x1)*(y-y1)); 
	// if(y1==y2) {
	// 	cout << coord[0] << " " << coord[1] << endl;
	// 	cout << x1 << " " << x2 << " " << width << endl;
	// 	cout << y1 << " " << y2 << " " << height << endl;
	// 	cout << pixQ11 << " " << pixQ12 << endl;
	// 	cout << pixQ21 << " " << pixQ22 << endl;
	// 	cout << bilerp << endl;
	// }
	return bilerp;
}

glm::dvec3 TextureMap::getPixelAt(int x, int y) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	const uint8_t *pixel = data.data() + ( x + y * width ) * 3;
	//return glm::dvec3(pixel[0], pixel[1], pixel[2]);
	return glm::dvec3((double)pixel[0]/255.0, (double)pixel[1]/255.0, (double)pixel[2]/255.0);
}

glm::dvec3 MaterialParameter::value(const isect& is) const
{
	if (0 != _textureMap)
		return _textureMap->getMappedValue(is.getUVCoordinates());
	else
		return _value;
}

double MaterialParameter::intensityValue(const isect& is) const
{
	if (0 != _textureMap) {
		glm::dvec3 value(
		        _textureMap->getMappedValue(is.getUVCoordinates()));
		return (0.299 * value[0]) + (0.587 * value[1]) +
		       (0.114 * value[2]);
	} else
		return (0.299 * _value[0]) + (0.587 * _value[1]) +
		       (0.114 * _value[2]);
}
