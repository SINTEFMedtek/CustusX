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

/**
 *
 * \brief Definition of characteristics for an Ultrasound Probe Sector.
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
		ssc::Vector3D getOrigin_u() const; ///< probe origin in image space u. (lower-left corner origin)
		ssc::DoubleBoundingBox3D getClipRect_u() const; ///< sector clipping rect in image space u. (lower-left corner origin)
		ssc::Vector3D mOrigin_p; ///< probe origin in pixel space p. (upper-left corner origin)
		ssc::Vector3D mSpacing;
		ssc::DoubleBoundingBox3D mClipRect_p; ///< sector clipping rect, in addition to the standard sector definition. The probe sector is the intersection of the sector definition and the clip rect.
		QSize mSize;
		Vector3D transform_p_to_u(const Vector3D& q_p) const;

		void addXml(QDomNode dataNode) const; ///< write to xml
		void parseXml(QDomNode dataNode);///< load from xml
	};

	enum TYPE { tNONE=0,   ///< undefined
				tSECTOR=1, ///< US beam is emitted radially in a flat cone. 
				tLINEAR=2  ///< US beam is emitted straight forward.
				};

public:
	ProbeData(TYPE type = tNONE);
	void addXml(QDomNode dataNode) const; ///< write to xml
	void parseXml(QDomNode dataNode);///< load from xml

	TYPE getType() const;
	double getDepthStart() const;
	double getDepthEnd() const;
	double getWidth() const;
	double getTemporalCalibration() const;
	double getCenterOffset() const;
	ProbeImageData getImage() const;
	void resample(QSize mSize); ///< Set a new image size. Resample all other parameters to match this new image size, keeping sizes in millimeters fixed.
	QString getUid() const;

	void setTemporalCalibration(double value);
	void setType(TYPE type);
	void setImage(ProbeImageData value);
	void setSector(double depthStart, double depthEnd, double width, double centerOffset = 0);
	void setUid(QString uid);
	QString getUid();

	void updateClipRectFromSector();
	void updateSectorFromClipRect();
	void applySoundSpeedCompensationFactor(double factor);///< Compensate for soundspeed for linear probes

private:
	TYPE mType; ///< type of probe
	double mDepthStart; ///< start of sector, mm
	double mDepthEnd; ///< end of sector, mm
	double mWidth; ///< width of sector in radians (SECTOR) or millimeters (LINEAR).
	double mTemporalCalibration;
	double mCenterOffset; ///< Offset of center point for sectors, mm
	ProbeImageData mImage;
	double mSoundSpeedCompensationFactor; ///< The used sound speed compensation factor
	QString mUid; ///<  the uid of the stream data this probe data applies to.
};

typedef boost::shared_ptr<ProbeData> ProbeDataPtr;

} // namespace ssc

#endif /*SSCPROBEDATA_H_*/
