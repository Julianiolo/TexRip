#include "utils.h"
#include "Eigen/Dense"

Eigen::Matrix<double, 3, 3> get_homography_E(const Eigen::Matrix<double, 4, 2>& src_pts, const Eigen::Matrix<double, 4, 2>& dst_pts) //https://medium.com/all-things-about-robotics-and-computer-vision/homography-and-how-to-calculate-it-8abf3a13ddc5
{
	Eigen::Matrix<double, 8, 9> A;

	for(int i=0; i<4; i++)
	{
		Eigen::Matrix<double, 9, 1> ax;
		Eigen::Matrix<double, 9, 1> ay;

		ax << src_pts(i,0), src_pts(i,1), 1.,              0.,              0., 0., -1*src_pts(i,0)*dst_pts(i,0), -1*src_pts(i,1)*dst_pts(i,0), -1*dst_pts(i,0);
		ay <<           0.,           0., 0., -1*src_pts(i,0), -1*src_pts(i,1), -1,    src_pts(i,0)*dst_pts(i,1),    src_pts(i,1)*dst_pts(i,1),    dst_pts(i,1);

		A.row(2*i) = ax;
		A.row(2*i+1) = ay;
	}

	Eigen::JacobiSVD<Eigen::Matrix<double, 8, 9>> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);

	Eigen::Matrix<double, 9, 9> V = svd.matrixV();
	Eigen::Matrix<double, 9, 1> h = V.col(8);

	Eigen::Matrix<double, 3, 3> homography;
	homography <<	h(0,0), h(1,0), h(2,0),
		h(3,0), h(4,0), h(5,0),
		h(6,0), h(7,0), h(8,0);

	return homography;
}

void utils::getHomography(const Vector2* src, const Vector2* dst, float* mat) {
	Eigen::Matrix<double, 4, 2> srcM, dstM;
	srcM << src[0].x, src[0].y,
		src[1].x, src[1].y,
		src[3].x, src[3].y,
		src[2].x, src[2].y;

	dstM << dst[0].x, dst[0].y,
		dst[1].x, dst[1].y,
		dst[3].x, dst[3].y, 
		dst[2].x, dst[2].y;

	Eigen::Matrix<double, 3, 3> res = get_homography_E(srcM, dstM);
	for (int i = 0; i < 9; i++) {
		mat[i] = (float)res(i%3,i/3);
	}
}

