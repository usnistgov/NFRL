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
#pragma once

#include "points_on_image.h"


namespace NFRL {

/**
 * Each image has one pair of points that correspond to the other (image).
 * These are not the corresponding point pairs but are the points on the same
 * image and the Euclidean distance between them is the "segment".
 * 
 * These points are passed into the constructor, and the constructor calculates
 * the scale factor(sf) where sf is the ratio of the segments.
 */
class PointsOnImages
{

public:
  // Full constructor.
  PointsOnImages( NFRL::PointsOnImage &, NFRL::PointsOnImage & );

  // Virtual destructor.
  virtual ~PointsOnImages() {}

  double getScaleFactor();
  std::string to_s();

private:
  /** First and second points on Moving image. */
  NFRL::PointsOnImage &_pairMoving;
  /** First and second points on Fixed image. */
  NFRL::PointsOnImage &_pairFixed;

  /** Ratio of image segment lengths */
  double _scaleFactor;

};

}   // End namespace
