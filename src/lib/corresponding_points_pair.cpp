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
#include "corresponding_points_pair.h"

namespace NFRL {

/** Initialization function that resets all values. */
void CorrespondingPointsPair::Init()
{
  _movingPt = cv::Point2f(0, 0);
  _fixedPt  = cv::Point2f(0, 0);
}

/** Copy function to make clones of the object. */
void CorrespondingPointsPair::Copy( const CorrespondingPointsPair& aCopy )
{
  _movingPt = aCopy._movingPt;
  _fixedPt = aCopy._fixedPt;
}

/** Default constructor.  Calls Init(). */
CorrespondingPointsPair::CorrespondingPointsPair()
{
  Init();
}

/** Copy constructor.  This is called when passing the object by value
 * as parameter to CorrespondingPointsPair constructor.
 * 
 * @param aCopy object to be copied
 */
CorrespondingPointsPair::CorrespondingPointsPair(
    const CorrespondingPointsPair& aCopy )
{
  Copy( aCopy );
}

/** Base class constructor.  These points are corresponding *across* images.
 *
 * @param movingPt (x,y) point coords in moving image
 * @param fixedPt (x,y) point coords in fixed image
 */
CorrespondingPointsPair::CorrespondingPointsPair( cv::Point2f movingPt,
                                                  cv::Point2f fixedPt )
{
  _movingPt = movingPt;
  _fixedPt = fixedPt;
}

/** Calculate the Euclidean distance between the point-pair.
 * 
 * @return double Euclidean distance
 */
double CorrespondingPointsPair::distance()
{
  cv::Point diff = _movingPt - _fixedPt;
  double dist = sqrt( diff.ddot(diff) );
  return dist;
}

/**
 * @return string of points-pair: `(x1,y1) X (x2,y2)`
 */
std::string CorrespondingPointsPair::to_s()
{
  std::string sout{""};
  int mx = static_cast<int>(_movingPt.x);
  int my = static_cast<int>(_movingPt.y);
  int sx = static_cast<int>(_fixedPt.x);
  int sy = static_cast<int>(_fixedPt.y);
  sout.append( "(" + std::to_string(mx) + ", " + std::to_string(my) + ")" );
  sout.append( " X " );
  sout.append( "(" + std::to_string(sx) + ", " + std::to_string(sy) + ")" );
  return sout;
}

}   // End namespace
