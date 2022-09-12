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

#include "exceptions.h"

typedef unsigned char uchar;

#include <map>
#include <string>
#include <vector>

/** Object of this type is used strictly for registration metadata. */
typedef std::vector<std::vector<float>> Rotate2D;
/** Object of this type is used strictly for registration metadata. */
typedef std::vector<std::vector<int>> Translate2D;
/** Registration metadata in XML format.  Each string in the vector is
 *  a correctly-formed XML text string.  The object of this type must be
 *  utilized in its entirety to obtain a complete XML doc.
 */
typedef std::vector<std::string> XmlMetadata;

#define NFRL_VERSION "0.1.0"


#ifdef USE_OPENCV
  namespace NFRL {
#else
  namespace NFRL_ITL {
#endif

/**
 * @return the current versions of this NFRL software and OpenCV
 */
std::string printVersion();


/**
 * This class implements the NIST Fingerprint Registration Library.
 * The caller instantiates this class and calls the performRegistration()
 * function to perform the entire registration process on a single pair of
 * images where the registration points are corresponding control points as
 * determined by the caller.
 * 
 * Each image used in a registration process may be referred to using multiple
 * terms: moving or source, and, target, fixed, or sensed.  For the purpose
 * of this discussion, the Fixed image remains fixed in 2-dimensional space
 * and the Moving image is “moved” to align, that is to register, with the
 * Fixed image.
 * 
 * The rigid-registration is performed in two steps in order:
 * 1. translate
 * 2. rotate.
 * 
 * Registered images are subsequently overlaid to determine the smallest
 * rectangle that contains the most amount of fingerprint common to both.
 * This Region of Interest (rectangle) is the area that is used to crop the
 * moving and fixed images.
 * 
 * These images are generated:
 *   - Moving image that is cropped and registered to the Fixed image
 *   - cropped, Fixed image
 *   - padded, registered Moving image, grayscale
 *   - padded, Fixed image, grayscale (per the registration)
 *   - overlaid padded and registered images, in color, for visual inspection
 *     of registration result.
 */
class Registrator
{
private:
  /** Byte-stream of the Moving image. */
  std::vector<uchar> _imgMoving;
  /** Byte-stream of the Fixed image. */
  std::vector<uchar> _imgFixed;
  /** 8 individual coordinates of the two registration pairs of points. */
  std::vector<int> &_correspondingPoints;
  /** Each run of the registration process captures metadata for use by
   *  the caller. */
  std::vector<std::string> &_metadata;

  /** Byte-stream of the registered, cropped, Moving image. */
  std::vector<uchar> _vecCroppedRegisteredImage;
  /** Byte-stream of the registered, cropped, Fixed image. */
  std::vector<uchar> _vecCroppedFixedImage;
  /** Byte-stream of the padded, overlaid, registered images as a single image.
   */
  std::vector<uchar> _vecColorOverlaidRegisteredImages;
  /** Byte-stream of padded, registered, Fixed image, grayscale. */
  std::vector<uchar> _vecPaddedFixedImg;
  /** Byte-stream of padded, registered, Moving image, grayscale. */
  std::vector<uchar> _vecPaddedRegisteredMovingImg;

  // Registration-process imagery
  /** Byte-stream of padded, yet to be registered, Moving image. */
  // std::vector<uchar> _vecPaddedUnregisteredMovingImg;
  /** Byte-stream of padded, yet to be registered, Fixed image. */
  // std::vector<uchar> _vecColorPaddedUnregisteredFixedImg;

  // Debug imagery
  /** Byte-stream of blob of overlay region only. */
  std::vector<uchar> _vecPngBlob;

  /** Supports padding of source images prior to registration. */
  struct PaddingDifferential
  {
    /** Margin from top of source image to padded edge. */
    int top;
    /** Margin from bottom of source image to padded edge. */
    int bot;
    /** Margin from left of source image to padded edge. */
    int left;
    /** Margin from right of source image to padded edge. */
    int right;

    /** Reset all pad values to zero. */
    void reset()
    {
      top = 0; bot = 0; left = 0; right = 0;
    }
  }
  /** Container for padding values (all 4 sides) for the Moving image. */
  _padDiffMoving,
  /** Container for padding values (all 4 sides) for the Fixed image. */ 
  _padDiffFixed;


  /** This struct supports registration metadata capture. */
  struct Point
  {
    /** x-coord of a point */
    int x;
    /** y-coord of a point */
    int y;
    /** Convert the coords to a comma-separated string. */
    std::string to_s()
    {
      std::string s{""};
      s = std::to_string(x);
      s.append(",");
      s.append(std::to_string(y));
      return s;
    }
    /** Convert the coords to vector of ints: in order x, y. */
    void to_v( std::vector<int> &center )
    {
      center.clear();
      center.push_back( x );
      center.push_back( y );      
    }
  };


  /** This struct supports registration metadata capture. */
  struct ImageSize
  {
    int width;
    int height;

    std::string getWidth()
    {
      return std::to_string( width );
    }
    std::string getHeight()
    {
      return std::to_string( height );
    }
    void set( int w, int h )
    {
      width = w; height = h;
    }
    /**
    @return WxH as string
    */
    std::string getWxH()
    {
      std::string s{""};
      s = std::to_string( width );
      s.append("x");
      s.append( std::to_string( height) );
      return s;
    }
  };


  /** Support for registration metadata scale factor (sf) calculation. */
  enum ScaleFactorDirection
  {
    /** sf = img1/img2 (default) */
    img1_to_img2 = 1,
    /** sf = img2/img1 */
    img2_to_img1
  };


public:
  /**
   * This struct is used to capture registration metadata calculated each time
   * a pair of images are registered.  The user of this NFRL library has access
   * to the metadata per this custom type.  Also, the metadata is injected into
   * XML that is output as a vector of strings
   * (see Registrator::getXmlMetadata( XmlMetadata ) ).
   */
  struct RegistrationMetadata
  {
    // ----- Translation -----
    /** Translation in x-direction. */
    int tx{0};
    /** Translation in y-direction. */
    int ty{0};

    /** Loaded using CVops::cast_translation_matrix() method. */
    Translate2D translMatrix;

    /**
     * Retrieve the translation transform as a vector.  Each element of the
     * returned vector is one row of the transform.
     * 
     * @return vector with each row of the transform in each element
     */
    std::vector<std::string> getTranslationTransform()
    {
      size_t tRows{translMatrix.size()};
      size_t tCols{translMatrix[1].size()};
      std::vector<std::string> v;

      std::string sTmp{""};
      std::string sMatrix{""};
      for( size_t i=0; i<tRows; i++ )
      {
        for( size_t j=0; j<tCols; j++ )
        {
          sTmp = std::to_string( translMatrix[i][j] );
          sMatrix.append(sTmp);
          if( j < tCols-1 )
            sMatrix.append(" ");
        }
        v.push_back( sMatrix );
        sMatrix = "";   // clear it for next row
      }
      return v;
    }

    // ----- Rotation -----
    /** Angle between segments on each image. */
    double angleDiffDegrees{0.0};

    /**
     * Center of rotation, by design, is the first control point on the Fixed
     * image.  This is the point to which the Moving image is translated.
     */
    Point centerRot;

    /**
     * Retrieve the center of rotation point as a vector.  This is
     * essentially the point on the Fixed image to which the Moving image
     * was rigidly translated.  The registration is completed by then
     * rotating the Moving image about this point.
     * 
     * @param center IN OUT vector with two elements: in order x,y
     */
    void centerOfRotation( std::vector<int> &center )
    {
      centerRot.to_v( center );
    }

    /** Loaded using CVops::cast_rotation_matrix() method. */
    Rotate2D rotMatrix;

    /**
     * Retrieve the rotation transform as a vector.  Each element of the
     * returned vector is one row of the transform.
     * 
     * @return vector with each row of the transform in each element
     */
    std::vector<std::string> getRotationTransform()
    {
      size_t tRows{rotMatrix.size()};
      size_t tCols{rotMatrix[1].size()};
      std::vector<std::string> v;

      std::string sTmp{""};
      std::string sMatrix{""};
      for( size_t i=0; i<tRows; i++ )
      {
        for( size_t j=0; j<tCols; j++ )
        {
          sTmp = std::to_string( rotMatrix[i][j] );
          sMatrix.append(sTmp);
          if( j < tCols-1 )
            sMatrix.append(" ");
        }
        v.push_back( sMatrix );
        sMatrix = "";   // clear it for next row
      }

      return v;
    }

    // ----- Control points -----
    /**
     * Four points of the registered images after the registration, two on
     * each image. The key of the map-container is "ptX" where X is the point
     * number. The Euclidean distances are those between the control points.
     */
    struct ControlPoints
    {
      /**
       * Each of the four control points numbered, eg, point #1 = (123, 456).
       */
      std::map<std::string, Point> point;
      /**
       * Euclidean distances between tranformed points (post registration).
       * The unconstrained pair is that used for translation.
       * The constrained pair is that used for rotation.
       */
      struct EuclideanDistance
      {
        /** Distance between constrained pair of points (across images, the two
        points used for rotation). */
        double constrained{0.0};
        /** Distance between unconstrained pair of points (across images,
         * the two points used for translation). */
        double unconstrained{0.0};

        /** Convert the constrained-points distance to string: always zero. */
        std::string to_s_constrained()
        {
          std::string s{std::to_string(constrained)};
          return s;
        }
        /** Convert the unconstrained-points distance to string. */
        std::string to_s_unconstrained()
        {
          std::string s{std::to_string(unconstrained)};
          return s;
        }
      }
      /** Euclidean distances between tranformed points (post registration). */
      euclideanDistance;

      /** Get the point as string in point-format.
       * 
       * @param pointNum either of 1 | 2 | 3 | 4
       */
      std::string getControlPoint( short pointNum )
      {
        std::string key = "pt" + std::to_string(pointNum);
        auto p = point[key];
        return p.to_s();
      }

      /** Set the point coordinates and push into map of control points.
       * 
       * @param pointNum either of 1 | 2 | 3 | 4
       * @param x coordinate
       * @param y coordinate
       */
      void setControlPoint( short pointNum, int x, int y )
      {
        Point p;
        p.x = x;
        p.y = y;
        std::string key = "pt" + std::to_string(pointNum);
        point[key] = p;
      }
    }
    /** Container for four points of the registered images after the
     *  registration. */
    controlPoints;

    // ----- Scale factor
    /** Ratio of segment lengths and "direction" of registration. */
    struct ScaleFactor
    {
      /** Ratio of segment lengths. */
      double value;
      /** Which image segment length is in the denominator of ratio, ie,
       *  image1 or image2.
       */
      ScaleFactorDirection direction;

      /** "Direction" of the ratio.
       * 
       *  @return [ img1/img2 | img2/img1 ]
       */
      std::string getScaleFactorDirection()
      {
        std::string s{""};
        switch( direction )
        {
          case img1_to_img2 : s = "img1/img2"; break;
          case img2_to_img1 : s = "img2/img1"; break;
          default           : s = "undefined"; break;
        }
        return s;
      }
    }
    /** Container for ratio of segment lengths and 'direction' of registration.
     */
    scaleFactor;

    // ----- Report image sizes to caller
    /** W x H */
    ImageSize srcMovingImgSize;
    /** W x H */
    ImageSize srcFixedImgSize;
    /** W x H */
    ImageSize paddedImgSize;
    /** W x H */
    ImageSize registeredImgSize;

    // ----- Convert source image to grayscale
    /** Notification for caller that a source image has been converted to
     *  grayscale in order to continue the registration process. */
    struct ConvertToGrayscale
    {
      /** Set to true if image was converted to grayscale. */
      bool img1{false};
      /** Set to true if image was converted to grayscale. */
      bool img2{false};

      /** Quick check if either image was converted. */
      bool any() {
        if( img1 || img2 )
          return true;
        else
          return false;
      }

      /** @return "YES" if image was converted to grayscale, "NO" otherwise. */
      std::string img1_to_s()
      {
        if( img1 ) return "YES";
        else return "NO";
      }

      /** @return "YES" if image was converted to grayscale, "NO" otherwise. */
      std::string img2_to_s()
      {
        if( img2 ) return "YES";
        else return "NO";
      }

    }
    /** Container for notification for caller that a source image has been
     *  converted to grayscale. */
    convertToGrayscale;

    /** Top-left and bottom-right of ROI rectangle. */
    std::vector<std::string> overlapROICorners;

  }
  /** Container that captures registration metadata for each registration. */
  registrationMetadata;


protected:

  void Init();
  void Copy( const Registrator& );

public:
  // Default constructor.
  Registrator();

  // Copy constructor.
  Registrator( const Registrator& );

  // Full constructor.
  Registrator( std::vector<uchar>, std::vector<uchar>,
               std::vector<int> &, std::vector<std::string> & );

  // Virtual destructor.
  virtual ~Registrator() {}

  // Implement a clone operator.
  // Registrator Clone(void);

  // Implement an assigment operator.
  // Registrator operator=( const Registrator& );

  /**
   * This is the function that performs the registration. It uses the private
   * member variables of the two images and the corresponding control points
   * vector.
   */
  void performRegistration();

  std::vector<uchar> getColorOverlaidRegisteredImages();
  std::vector<uchar> getCroppedRegisteredImage();
  std::vector<uchar> getCroppedFixedImage();
  std::vector<uchar> getPaddedFixedImg();
  std::vector<uchar> getPaddedRegisteredMovingImg();
  std::vector<uchar> getPngBlob();


  void getMetadata( RegistrationMetadata& );

  /**
   * Builds the XML nodes and inserts relevant registration metadata.
   */
  void getXmlMetadata( XmlMetadata& );

  /**
   * The registered image is located in memory. This function gives the using
   * software the option to save image to disk.
   */
  void saveCroppedRegisteredImageToDisk( const std::string path );

  /**
   * The registered image is located in memory. This function gives the using
   * software the option to save image to disk.
   */
  void saveCroppedFixedImageToDisk( const std::string path );

  /**
   * Interface to the private container.
   * 
   * @return current padding object for Moving image
   */
  PaddingDifferential getPadDiffMoving()
  {
      return _padDiffMoving;
  }
  /**
   * Interface to the private container.
   * 
   * @return current padding object for Fixed image
   */
  PaddingDifferential getPadDiffFixed()
  {
      return _padDiffFixed;
  }

private:
  void buildXmlTagline( XmlMetadata&, std::string );
  void buildXmlTagline( XmlMetadata&, std::string, std::string );

};

}   // END namespace
