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
#include "corresponding_points_pairs.h"

// #include <iostream>

namespace NFRL {

/** Initialization function that resets all values. */
void CorrespondingPointsPairs::Init()
{
  _pair1 = NFRL::CorrespondingPointsPair( cv::Point2f(0, 0), cv::Point2f(0, 0) );
  _pair2 = NFRL::CorrespondingPointsPair( cv::Point2f(0, 0), cv::Point2f(0, 0) );
}

/** Copy function to make clones of an object. */
void CorrespondingPointsPairs::Copy( const CorrespondingPointsPairs& aCopy )
{
  _pair1 = aCopy._pair1;
  _pair2 = aCopy._pair2;
}

/** Default constructor.  Calls Init(). */
CorrespondingPointsPairs::CorrespondingPointsPairs()
{
  Init();
}

/** Copy constructor.  This is called when passing the object by value
 * as parameter to CorrespondingPointsPairs constructor.
 * 
 * @param aCopy object to be copied
 */
CorrespondingPointsPairs::CorrespondingPointsPairs(
    const CorrespondingPointsPairs& aCopy )
{
  Copy( aCopy );
}

/** Base class constructor.
 *
 * @param pair1 as input into the registion process (method-call)
 * @param pair2 as input into the registion process (method-call)
 */
CorrespondingPointsPairs::CorrespondingPointsPairs(
    NFRL::CorrespondingPointsPair pair1, NFRL::CorrespondingPointsPair pair2 )
{
  _pair1 = pair1;
  _pair2 = pair2;
}

/**
 * @return string of points-pairs: `#1: (x1,y1) * (x2,y2)  #2: (x3,y3) * (x4,y4)`
 */
std::string CorrespondingPointsPairs::to_s()
{
  std::string out;
  std::string s1 = _pair1.to_s();
  std::string s2 = _pair2.to_s();

  out = "  #1: " + s1 + "  #2: " + s2;
  return out;
}

}   // End namespace
