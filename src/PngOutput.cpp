/// @file PngOutput.cpp  @version 1.0 @date 08/21/2012
/// @author BlahGeek@Gmail.com

#include "PngOutput.h"
#include <cmath>
#include <cstdio>
#include <cassert>
#include <iostream>
#include "common.hpp"
using namespace std;
using cv::Vec3b;

PngOutput::PngOutput(int width, int height, const char *filename):Output(width, height){
    this->filename = filename;
    img.create(height, width, CV_8UC3);
}

void PngOutput::draw(int x, int y, Color color){
    Vec3b &pixel = img.at<Vec3b>(y, x);
    double exposure = -1.0 / 255.0;
	double a, b, c;
	pixel[2] = uint8_t(a = 255.0 * (1.0 - expf(color.x * exposure)));
	pixel[1] = uint8_t(b = 255.0 * (1.0 - expf(color.y * exposure)));
	pixel[0] = uint8_t(c = 255.0 * (1.0 - expf(color.z * exposure)));

	//std::cerr << a << ", " << b << ", " << c << std::endl;
//	memcpy(rows[y]+ x * 3, &color, 3);
}

void PngOutput::finish(){
    imwrite(filename.c_str(), img);
}
