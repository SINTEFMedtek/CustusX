/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIGTLINKCONVERSIONSONIXCXLEGACY_H
#define CXIGTLINKCONVERSIONSONIXCXLEGACY_H


#include "igtlStringMessage.h"
#include "igtlStatusMessage.h"
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"

#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxTool.h"
#include "cxOpenIGTLinkUtilitiesExport.h"

namespace cx
{

class cxOpenIGTLinkUtilities_EXPORT IGTLinkConversionSonixCXLegacy
{
public:
	/**
	  * Encode the image into a IGTLink message, containing
	  * image data, uid and timstamp
	  */
	igtl::ImageMessage::Pointer encode(ImagePtr image);
	/**
	  * Decode the IGTLink message to create an image containing
	  * image data, uid and timstamp. The color format is also
	  * converted to RGBX
	  */
//	ImagePtr decode(IGTLinkImageMessage::Pointer msg);
	ImagePtr decode(igtl::ImageMessage::Pointer msg);

	/**
	  * Decode the image to standard format with standard color RGBX encoding.
	  *
	  * Find the substring [XYZW] in the msg uid, where each letter can be
	  * one of RGBAX. The letters describe the image components. Rearrange
	  * to standard RGBX format, strip format from uid,
	  * and return as new image.
	  */
	ImagePtr decode(ImagePtr msg);

	ProbeDefinitionPtr decode(ProbeDefinitionPtr msg);

    /**
     * Return true if the device name if on a format indicating that the message is legacy.
     */
    bool guessIsSonixLegacyFormat(QString deviceName);

private:	/** Extract the color format string from enclosing brackets inside
	  * another string, i.e find "RGBA" from "Device[RGBA]".
	  * Also return the input without format string as cleanedDeviceName.
	  */
	QString extractColorFormat(QString deviceName, QString* cleanedDeviceName);
	/** Filter that converts to RGB format based on a format string
	  * of the form "RGBA" or any other ordering of these four letters,
	  * the letters define the ordering of channels in the input.
	  */
	vtkImageDataPtr createFilterFormat2RGB(QString format, vtkImageDataPtr input);
	/** Filter that converts from a XYZW-format to RGB.
	  * The input indexes are the indexes or red/green/blue in the input.
	  * The alpha channel is discarded.
	  */
	vtkImageDataPtr createFilterAny2RGB(int R, int G, int B, vtkImageDataPtr input);

};

} //namespace cx

#endif // CXIGTLINKCONVERSIONSONIXCXLEGACY_H
