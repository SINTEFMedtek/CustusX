/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXCREATEPROBEDEFINITIONFROMCONFIGURATION_H_
#define CXCREATEPROBEDEFINITIONFROMCONFIGURATION_H_

#include "cxResourceExport.h"

#include "cxProbeDefinition.h"
#include "ProbeXmlConfigParser.h"

/**
* \file cxCreateProbeDefinitionFromConfiguration.h
*
* \date Nov 26, 2010
* \author Christian Askeland, SINTEF
*
* \addtogroup cx_resource_core_tool
* @{
*/

namespace cx
{



/**
 * Convert from ProbeXmlConfigParser::Configuration to ProbeDefinition
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
cxResource_EXPORT ProbeDefinition createProbeDefinitionFromConfiguration(ProbeXmlConfigParser::Configuration config);

/**
 * \sa ProbeXmlConfigParser
 * \sa createConfigurationFromProbeDefinition()
 */
cxResource_EXPORT ProbeXmlConfigParser::Configuration createConfigurationFromProbeDefinition(ProbeXmlConfigParser::Configuration basis, ProbeDefinition data);

} // namespace cx

/**
* @}
*/

#endif /* CXCREATEPROBEDEFINITIONFROMCONFIGURATION_H_ */
