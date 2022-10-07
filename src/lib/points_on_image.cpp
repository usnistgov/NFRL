/*******************************************************************************
License:
This software was developed at the National Institute of Standards and
Technology (NIST) by employees of the Federal Government in the course
of their official duties. Pursuant to title 17 Section 105 of the
United States Code, this software is not subject to copyright protection
and is in the public domain. NIST assumes no responsibility  whatsoever for
its use by other parties, and makes no guarantees, expressed or implied,
about its quality, reliability, or any other characteristic.

This software has been determined to be outside the scope of the EAR
(see Part 734.3 of the EAR for exact details) as it has been created solely
by employees of the U.S. Government; it is freely distributed with no
licensing requirements; and it is considered public domain. Therefore,
it is permissible to distribute this software as a free download from the
internet.

Disclaimer:
This software was developed to promote biometric standards and biometric
technology testing for the Federal Government in accordance with the USA
PATRIOT Act and the Enhanced Border Security and Visa Entry Reform Act.
Specific hardware and software products identified in this software were used
in order to perform the software development.  In no case does such
identification imply recommendation or endorsement by the National Institute
of Standards and Technology, nor does it imply that the products and equipment
identified are necessarily the best available for the purpose.
*******************************************************************************/
#include "points_on_image.h"

namespace NFRL {

/** @brief Initialization function that resets all values. */
void PointsOnImage::Init() {
  _pointOne = cv::Point2f(0, 0);
  _pointTwo = cv::Point2f(0, 0);
  _slope = 0;
  _sideX = -1;
  _sideY = -1;
}

/** @brief Supports copy-constructor. */
void PointsOnImage::Copy( const PointsOnImage& aCopy )
{
  _pointOne = aCopy._pointOne;
  _pointTwo = aCopy._pointTwo;
}

/** @brief Default constructor. Calls Init(). */
PointsOnImage::PointsOnImage()
{
  Init();
}

/** @brief Copy constructor.
 *
 * This is called when passing the object by value as parameter to
 * PointsOnImages constructor.
 */
PointsOnImage::PointsOnImage( const PointsOnImage& aCopy )
{
  Copy( aCopy );
}

/**
 * @brief Calculate the angle between the segment defined by the two points
 *  and the horizontal.
 *
 * The horizontal is defined by standard Cartesian coordinates:
 *    from 0 to positive X.
 * 
 * Point x-coordinate: parallel to the horizontal.
 * Point y-coordinate: parallel to the vertical.
 * 
 * Since image origin (0, 0) is the top-left corner, all point coordinates
 * are positive, and therefore x increases to the right and y increases down.
 * 
 * Because the image origin is located at top-left, the slope of the segment
 * is the negative of the calculated slope.
 * 
 * Calculations are based on the following:
 * 
 * 1. Point 1 to Point 2 defines the segment; think of it as a "ray"
 * 2. Point 1 is defined as the "origin" of the ray
 * 3. For the OpenCV rotation matrix calculation, Point 2 is rotated to align
 *    with Point 1.
 * 
 * There are 4 possible segment displacements. Slope and angle are defined
 * as follows:
 * 1. Point 2 below and to the right of Point 1; slope defined as negative,
 *    angle from horizontal defined as negative
 * 2. Point 2 above and to the right of Point 1; slope is positive,
 *    angle is positive
 * 3. Point 2 above and to the left of Point 1; slope is negative,
 *    angle is positive
 * 4. Point 2 below and to the left of Point 1; slope is positive,
 *    angle is negative
 * 
 * The range of cosine is 0-180 degrees, but 4 angles must be taken into
 * account, 2 positive and 2 negative.
 * 
 * Consequently, the opposite of the calculated angle must be used based
 * on the sign of sideX and the slope.
 * 
 * Note there is no assertion for potential divide-by-zero error.  This should
 * never occur because Miscue is thrown prior to this constructor being called
 * when control-point-pair for "same" image (vs across images) contains the
 * identical point (x,y) coordinates.
 * 
 * @param pointOne (x,y) first-selected point on image
 * @param pointTwo (x,y) second-selected point on image
 */
PointsOnImage::PointsOnImage( cv::Point2f pointOne, cv::Point2f pointTwo )
{
  _pointOne = pointOne;
  _pointTwo = pointTwo;

  _sideX = _pointTwo.x - _pointOne.x;
  _sideY = _pointTwo.y - _pointOne.y;
  _slope = (float)_sideY / (float)_sideX;
  _slope = -_slope;    // image origin is located at top-left
  segmentLength = sqrt( std::pow(_sideX, 2) + std::pow(_sideY, 2) ) ;

  angleDegrees = std::acos( _sideX / segmentLength ) * 180.0 / 3.141592653589;
  if( ( _sideX > 0.0 ) && ( _slope < 0 ) )
    angleDegrees = -angleDegrees;
  else if( ( _sideX < 0.0 ) && ( _slope > 0 ) )
    angleDegrees = -angleDegrees;
}

/**
 * @brief All metadata per registration.
 *
 * @param kind [ moving | fixed ] image
 *
 * @return single string (with trailing `\n`) of all relevant info
 */
std::string PointsOnImage::to_s( const std::string &kind ) const
{
  std::string str0{""}, str1{""}, str2{""}, str3{""};
  str0 = "(" + std::to_string(_pointOne.x) + ", "
       + std::to_string(_pointOne.y) + ") * (" + std::to_string(_pointTwo.x)
       + ", " + std::to_string(_pointTwo.y) + ")\n";
  str1 = kind + " _sideX: " + std::to_string(_sideX)
       + ", _sideY: " + std::to_string(_sideY) + ", segmentLength: "
       + std::to_string(segmentLength) + "\n";
  str2 = kind + " SLOPE: " + std::to_string(_slope) + "\n";
  str3 = kind + " Angle from horizontal: " + std::to_string(angleDegrees)
       + " degrees\n";
  return str0 + str1 + str2 + str3;
}

/** @brief The points-pair on the (same) image.
 *
 * @return points on image in a vector container
 */
std::vector<cv::Point2f> PointsOnImage::getVectorOfPoints() const
{
  std::vector<cv::Point2f> v;
  v.push_back( _pointOne );
  v.push_back( _pointTwo );
  return v;
}

}   // End namespace
