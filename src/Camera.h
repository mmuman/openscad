#pragma once

/*

Camera

For usage, see QGLView.cc, GLView.cc, export_png.cc, openscad.cc

There are two different types of cameras represented in this class:

*Gimbal camera - uses Euler Angles, object translation, and viewer distance
*Vector camera - uses 'eye', 'center', and 'up' vectors ('lookat' style)

They are not necessarily kept in sync. There are two modes of
projection, Perspective and Orthogonal.

*/

#include "linalg.h"
#include <vector>
#include <Eigen/Geometry>

class Camera
{
public:
	enum CameraType { NONE, GIMBAL, VECTOR } type;
	enum ProjectionType { ORTHOGONAL, PERSPECTIVE } projection;
	Camera(enum CameraType camtype = NONE);
	void setup(std::vector<double> params);
	void gimbalDefaultTranslate();
	void viewAll(const BoundingBox &bbox, float scalefactor = 1.0f);

	// Vectorcam
	Eigen::Vector3d eye;
	Eigen::Vector3d center; // (aka 'target')
	Eigen::Vector3d up; // not used currently

	// Gimbalcam
	Eigen::Vector3d object_trans;
	Eigen::Vector3d object_rot;
	double viewer_distance;

  // Perspective settings
	double fov; // Field of view

  // Orthographic settings
	double height; // world-space height of viewport

	bool viewall;

	unsigned int pixel_width;
	unsigned int pixel_height;
};
