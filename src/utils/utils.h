#ifndef _UTILS
#define _UTILS
#include "raylib.h"
#include <string>

namespace utils {
	Vector2 rotatePoint(Vector2 pnt, const Vector2& center, float s, float c);

	void getHomography(const Vector2* src, const Vector2* dst, float* mat);

	std::string getKeyStr(int key);
}

//Eigen::Matrix<double, 3, 3> get_homography_E(const Eigen::Matrix<double, 4, 2>& video_pts, const Eigen::Matrix<double, 4, 2>& logo_pts);

#endif

