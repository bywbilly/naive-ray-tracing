/// @file common.hpp  @version 1.0 @date 06/10/2013
/// @author BlahGeek@Gmail.com
/// @brief Some typedef
#ifndef H_COMMON_
#define H_COMMON_

#include "opencv2/core/core.hpp"
#include <limits>


#define EPSILON 1e-8
#define NOT_INTERSECT -1

typedef cv::Point3_<double> Vec;
typedef cv::Point3_<double> Color;

#define SIGN(x) ((x)>-EPSILON && (x)<EPSILON)

#endif

