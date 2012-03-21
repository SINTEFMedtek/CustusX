// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCPROBEDATA_H_
#define SSCPROBEDATA_H_

#include <QSize>
class QDomNode;
#include "sscVector3D.h"
#include "sscBoundingBox3D.h"

namespace ssc
{

/**\brief Definition of characteristics for an Ultrasound Probe Sector.
 *
 *  \verbatim
 *
 *  * Definitions of probe geometry:
 *
 *  Sector probe:
 *
 *                    *   -------------- origin of t space
 *                  '   '
 *                '       '
 *              '           '
 *            '  '         '  '
 *          '      ' ' ' '      '  ----- depth start
 *        '                       '
 *      '                           '
 *        ' ' '               ' ' '
 *              ' ' ' ' ' ' '  --------- depth end
 *
 *             width in radians
 *
 *
 * Linear probe:
 *
 *         ' ' ' ' ' '*' ' ' ' ' ------- depth start, origin (*) of t space
 *         '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         ' ' ' ' ' ' ' ' ' ' ' ------- depth end
 *
 *              width in mm
 *
 *
 * Associated image:
 *
 *            x                   coordinate space v: origin upper left corner
 *      v*---->
 *       |
 *       | ' ' ' ' ' ' ' ' ' ' '  size: pixel dimensions (width, height)
 *      y. '                   '  spacing: pixel size (width, height)
 *         '         *         '  origin_u: calibration center in space u, dimensions mm. origin of t space.
 *         '                   '
 *      y^ '                   '
 *       | '                   '
 *       | ' ' ' ' ' ' ' ' ' ' '
 *      u*---->                  coordinate space u: origin lower left corner
 *            x
 *
 * \endverbatim
 *
 * \ingroup sscTool
 */
class ProbeData
{
public: 
	struct ProbeImageData
	{
		ProbeImageData();
//    ssc::Vector3D mOrigin_u; ///< probe origin in image space u. (lower-left corner origin)
		ssc::Vector3D getOrigin_u() const; ///< probe origin in image space u. (lower-left corner origin)
		ssc::DoubleBoundingBox3D getClipRect_u() const; ///< sector clipping rect in image space u. (lower-left corner origin)
		ssc::Vector3D mOrigin_p; ///< probe origin in pixel space p. (upper-left corner origin)
		ssc::Vector3D mSpacing;
		ssc::DoubleBoundingBox3D mClipRect_p; ///< sector clipping rect, in addition to the standard sector definition. The probe sector is the intersection of the sector definition and the clip rect.
		QSize mSize;
		Vector3D transform_p_to_u(const Vector3D& q_p) const;
	};

	enum TYPE { tNONE=0,   ///< undefined
				tSECTOR=1, ///< US beam is emitted radially in a flat cone. 
				tLINEAR=2  ///< US beam is emitted straight forward.
				};	

public:
	ProbeData(TYPE type = tNONE);
//	ProbeData(double depthStart, double depthEnd, double width);
	void addXml(QDomNode& dataNode) const; ///< write internal state to node

	TYPE getType() const { return mType; }
	double getDepthStart() const { return mDepthStart; }
	double getDepthEnd() const { return mDepthEnd; }
	double getWidth() const { return mWidth; }
	double getTemporalCalibration() const { return mTemporalCalibration; }
	double getCenterOffset() const { return mCenterOffset; }
	ProbeImageData getImage() const { return mImage; }

	void setTemporalCalibration(double value) { mTemporalCalibration = value; }
//	void setImage(ProbeImageData value) { mImage = value; }
//	void setSector(double depthStart, double depthEnd, double width) { mDepthStart=depthStart; mDepthEnd=depthEnd; mWidth=width; }
	void setImage(ProbeImageData value);
	void setSector(double depthStart, double depthEnd, double width);

private:
	TYPE mType; ///< type of probe
	double mDepthStart; ///< start of sector, mm
	double mDepthEnd; ///< end of sector, mm
	double mWidth; ///< width of sector in radians (SECTOR) or millimeters (LINEAR).
	double mTemporalCalibration;
	double mCenterOffset; ///< Offset of center point for sectors, mm
	ProbeImageData mImage;
};

} // namespace ssc

#endif /*SSCPROBEDATA_H_*/
