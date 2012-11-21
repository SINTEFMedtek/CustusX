/*
 * cxCreateProbeDataFromConfiguration.h
 *
 *  \date Nov 26, 2010
 *      \author christiana
 */

#ifndef CXCREATEPROBEDATAFROMCONFIGURATION_H_
#define CXCREATEPROBEDATAFROMCONFIGURATION_H_

#include "sscProbeData.h"
#include "probeXmlConfigParser.h"

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

/**convert from ProbeXmlConfigParser::Configuration to ssc::ProbeData
 *
 * Notes about the conversion:
 * The xml format contains redundant data. The following list defines
 * which values in ProbeXmlConfigParser::Configuration that are actually used:
 * \verbatim

  BOTH:
    int                     mImageWidth;      ///< Width of the used image format (x dim)
    int                     mImageHeight;     ///< Height of the used image format (y dim)
    double                  mPixelWidth; ///<  Pixel width
    double                  mPixelHeight;///<  Pixel height
    float                   mOriginCol;  ///<  Origin.Col
    float                   mOriginRow;  ///<  Origin.Row
    QString                 mNotes; ///< useful information
    double                  mTemporalCalibration; ///< delay in timestamping in grabber source relative to master clock.

  NOT USED:
    int                     mNCorners;   ///<  number of corners
    std::vector<ColRowPair> mCorners;    ///<  corners <col,row>
    int                     mHorizontalOffset; ///< parameter for the grabber

  LINEAR:

  The linear probe defines a rectangle in pixels. Width/Depth are ignored in
  the file format, and created based on the edges:

    int                     mLeftEdge;   ///<  LeftEdge
    int                     mRightEdge;  ///<  RightEdge
    int                     mTopEdge;    ///<  TopEdge
    int                     mBottomEdge; ///<  BottomEdge


  SECTOR:

  The sector probe defines a sector AND a rectangle. The actual
  sector is the intersection between these entities.

    float                   mWidthDeg;   ///<  width in degrees
    float                   mDepth;      ///<  depth
    float                   mOffset;     ///<  Offset
    int                     mLeftEdge;   ///<  LeftEdge
    int                     mRightEdge;  ///<  RightEdge
    int                     mTopEdge;    ///<  TopEdge
    int                     mBottomEdge; ///<  BottomEdge

  \endverbatim
 *
 * \sa ProbeXmlConfigParser
 */
ssc::ProbeData createProbeDataFromConfiguration(ProbeXmlConfigParser::Configuration config);

/**
 * \sa ProbeXmlConfigParser
 * \sa createConfigurationFromProbeData()
 */
ProbeXmlConfigParser::Configuration createConfigurationFromProbeData(ProbeXmlConfigParser::Configuration basis, ssc::ProbeData data);

/**
* @}
*/

#endif /* CXCREATEPROBEDATAFROMCONFIGURATION_H_ */
