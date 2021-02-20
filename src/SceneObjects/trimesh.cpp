#include "trimesh.h"
#include <assert.h>
#include <float.h>
#include <string.h>
#include <algorithm>
#include <cmath>
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

using namespace std;

Trimesh::~Trimesh()
{
	for (auto m : materials)
		delete m;
	for (auto f : faces)
		delete f;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex(const glm::dvec3& v)
{
	vertices.emplace_back(v);
}

void Trimesh::addMaterial(Material* m)
{
	materials.emplace_back(m);
}

void Trimesh::addNormal(const glm::dvec3& n)
{
	normals.emplace_back(n);
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace(int a, int b, int c)
{
	int vcnt = vertices.size();

	if (a >= vcnt || b >= vcnt || c >= vcnt)
		return false;

	TrimeshFace* newFace = new TrimeshFace(
	        scene, new Material(*this->material), this, a, b, c);
	newFace->setTransform(this->transform);
	if (!newFace->degen)
		faces.push_back(newFace);
	else
		delete newFace;

	// Don't add faces to the scene's object list so we can cull by bounding
	// box
	return true;
}

// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
const char* Trimesh::doubleCheck()
{
	if (!materials.empty() && materials.size() != vertices.size())
		return "Bad Trimesh: Wrong number of materials.";
	if (!normals.empty() && normals.size() != vertices.size())
		return "Bad Trimesh: Wrong number of normals.";

	return 0;
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	bool have_one = false;
	for (auto face : faces) {
		isect cur;
		if (face->intersectLocal(r, cur)) {
			if (!have_one || (cur.getT() < i.getT())) {
				i = cur;
				have_one = true;
			}
		}
	}
	if (!have_one)
		i.setT(1000.0);
	return have_one;
}

bool TrimeshFace::intersect(ray& r, isect& i) const
{
	return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{
	// YOUR CODE HERE
	//
	// FIXME: Add ray-trimesh intersection
	const TrimeshFace *iFace = this;
	const glm::dvec3 tfNorm = normal; 
	if (abs(glm::dot(tfNorm,r.getDirection()))  <= 0) // check abs < kEpsilon instead
	 return false;

	glm::dvec3 a_coords = parent->vertices[ids[0]];
	glm::dvec3 b_coords = parent->vertices[ids[1]];
	glm::dvec3 c_coords = parent->vertices[ids[2]];
	glm::dvec3 midPoint =  (a_coords + b_coords + c_coords) * (1.0/3.0);
    
	//Point a b c
	glm::dvec3 area = glm::cross(a_coords-b_coords,c_coords-a_coords);
    double aArea = (1.0/2.0) * glm::length(area); 

	double tIntersect = (glm::dot(tfNorm,r.getPosition())+ glm::dot(tfNorm,midPoint))
	/glm::dot(tfNorm,r.getDirection());
	if (tIntersect<0) 
	return false;
	glm::dvec3 pointQ = r.getPosition() + (tIntersect * r.getDirection());
     // Do inside outside test to double-verify 
	 //(c-b) x (q-b) . n >=0 
	 //(a-c) x (q-c) . n >=0 
     bool insideOut = glm::dot(glm::cross(b_coords-a_coords,pointQ-a_coords),tfNorm) >= 0;
	 insideOut = insideOut && (glm::dot(glm::cross(c_coords-b_coords,pointQ-b_coords),tfNorm) >= 0);
	 insideOut = insideOut && (glm::dot(glm::cross(a_coords-c_coords,pointQ-c_coords),tfNorm) >= 0);
	 //if (!insideOut)
      //return false;
	
	 // using Point B , C and Point Q get a subarea
	glm::dvec3 aA = glm::cross(c_coords-b_coords,pointQ-b_coords);
    double xA = glm::length(aA) * (1.0/2.0);
	// using Point A , C and Point Q get b subarea
	glm::dvec3 aB = glm::cross(a_coords-c_coords,pointQ-c_coords);
    double xB = glm::length(aB) * (1.0/2.0);
	// using Point A , B and Point Q get c subarea 
	glm::dvec3 aC = glm::cross(b_coords-a_coords,pointQ-a_coords);
	double xC = glm::length(aC) * (1.0/2.0);
     
	double Qalpha = xA/aArea;
	double Qbeta  = xB/aArea;
	double Qgamma = xC/aArea; 

	bool inTriangle = (Qalpha >=0.0 && Qbeta>=0.0 && Qgamma >= 0.0) && (Qalpha+Qbeta+Qgamma == 1.0);
	//if (!inTriangle)
  	//return false;
    

    i.setT(tIntersect);
	i.setN(tfNorm);
	i.setMaterial(getMaterial());
	i.setUVCoordinates(glm::dvec2(Qalpha,Qbeta));
	i.setBary(Qalpha,Qbeta,Qgamma);
	// Check we are hitting the same plane as the triangle
	// check if we are hitting the triangle in that plane
    if (parent->vertNorms)
	{
     glm::dvec3 a_norm = parent->normals[ids[0]];
	 glm::dvec3 b_norm = parent->normals[ids[1]];
	 glm::dvec3 c_norm = parent->normals[ids[2]];

	 glm::dvec3 kd_Q = (Qalpha * a_norm) + (Qbeta * b_norm) + (Qgamma * c_norm);
	 kd_Q = glm::normalize(kd_Q);
	 //use interpolated normal instead 
	 i.setN(kd_Q);
	 parent->normals[ids[0]] = kd_Q;
	 parent->normals[ids[1]] = kd_Q;
	 parent->normals[ids[2]] = kd_Q;
    }
	return true;
}

void Trimesh::generateNormals()
{
	int cnt = vertices.size();
	normals.resize(cnt);
	std::vector<int> numFaces(cnt, 0);

	for (auto face : faces) {
		glm::dvec3 faceNormal = face->getNormal();

		for (int i = 0; i < 3; ++i) {
			normals[(*face)[i]] += faceNormal;
			++numFaces[(*face)[i]];
		}
	}

	for (int i = 0; i < cnt; ++i) {
		if (numFaces[i])
			normals[i] /= numFaces[i];
	}

	vertNorms = true;
}

