/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPROBEDATA_H_
#define CXPROBEDATA_H_

#include "cxResourceExport.h"

#include <QSize>
#include <QMap>
#include <QVariant>
class QDomNode;
#include "cxVector3D.h"
#include "cxBoundingBox3D.h"

namespace cx
{

/**
 *
 * \brief Definition of characteristics for an Ultrasound Probe Sector.
 *
 * NOTE: This documentation has been replaced by \ref cx_us_probe_definition.
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
 *            x                   coordinate space v (image): origin upper left corner, spacing in mm
 *      v*---->
 *       |
 *       | ' ' ' ' ' ' ' ' ' ' '
 *      y. '                   '  spacing in mm
 *         '    y<--t@         '  @ = point called origin: calibration center = origin of t (tool) space.
 *         '         |         '  (See also sscTool.h for definition of t space)
 *      y^ '        z.         '
 *       | '                   '
 *       | ' ' ' ' ' ' ' ' ' ' '
 *      u*---->                  coordinate space u (image): origin lower left corner
 *            x
 *
 *
 *
 *            x                   coordinate space p (image in pixels): origin upper left corner, spacing in pixels
 *      p*---->
 *       |
 *       | ' ' ' ' ' ' ' ' ' ' '  size: pixel dimensions (width, height)
 *      y. '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         ' ' ' ' ' ' ' ' ' ' '
 *
 *
 * \endverbatim
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ProbeDefinition
{
public: 
	enum TYPE { tNONE=0,   ///< undefined
				tSECTOR=1, ///< US beam is emitted radially in a flat cone. 
				tLINEAR=2  ///< US beam is emitted straight forward.
				};

public:
	ProbeDefinition(TYPE type = tNONE);
	void addXml(QDomNode dataNode) const;
	void parseXml(QDomNode dataNode);

	TYPE getType() const;
	double getDepthStart() const;
	double getDepthEnd() const;
	double getWidth() const;
	double getTemporalCalibration() const;
	double getCenterOffset() const;
	void resample(QSize mSize); ///< Set a new image size. Resample all other parameters to match this new image size, keeping sizes in millimeters fixed.
	QString getUid() const;

	void setTemporalCalibration(double value);
	void setType(TYPE type);
	void setSector(double depthStart, double depthEnd, double width, double centerOffset = 0);
	void setUid(QString uid);

	void updateClipRectFromSector();
	void updateSectorFromClipRect();
	void applySoundSpeedCompensationFactor(double factor);///< Compensate for soundspeed for linear probes
    double getSoundSpeedCompensationFactor() const;

	Vector3D getOrigin_u() const; ///< probe origin in image space u. (lower-left corner origin)
	DoubleBoundingBox3D getClipRect_u() const; ///< sector clipping rect in image space u. (lower-left corner origin)

	Vector3D getOrigin_p() const;
	Vector3D getSpacing() const;
	DoubleBoundingBox3D getClipRect_p() const;
	QSize getSize() const;

	void setOrigin_p(Vector3D origin_p);
	void setSpacing(Vector3D spacing);
	void setClipRect_p(DoubleBoundingBox3D clipRect_p);
	void setSize(QSize size);

	void setUseDigitalVideo(bool val); ///< RTSource is digital (eg. US sector is set digitally, not read from .xml file)
	bool getUseDigitalVideo() const;

	void setVariable(QString variableName, QVariant value);
	QVariant getVariable(QString variableName);

private:
	TYPE mType; ///< type of probe
	double mDepthStart; ///< start of sector, mm
	double mDepthEnd; ///< end of sector, mm
	double mWidth; ///< width of sector in radians (SECTOR) or millimeters (LINEAR).
	double mTemporalCalibration;
	double mCenterOffset; ///< Offset of center point for sectors, mm
	double mSoundSpeedCompensationFactor; ///< The used sound speed compensation factor
	QString mUid; ///<  the uid of the stream data this probe data applies to.

	//ProbeImageData variables
	Vector3D mOrigin_p; ///< probe origin in space p, measured in pixels. (upper-left corner origin)
	Vector3D mSpacing;//(_p)
	DoubleBoundingBox3D mClipRect_p; ///< sector clipping rect, in addition to the standard sector definition. The probe sector is the intersection of the sector definition and the clip rect.
	QSize mSize;//_p
	bool mDigital; ///< RTSource is digital (eg. US sector is set digitally, not read from .xml file)

	//ProbeImageData
	Vector3D transform_p_to_u(const Vector3D& q_p) const;
	void addImageXml(QDomNode dataNode) const;
	void parseImageXml(QDomNode dataNode);

	QMap<QString, QVariant> mAdditionalVariables;
};

typedef boost::shared_ptr<ProbeDefinition> ProbeDefinitionPtr;

} // namespace cx

#endif /*CXPROBEDATA_H_*/
