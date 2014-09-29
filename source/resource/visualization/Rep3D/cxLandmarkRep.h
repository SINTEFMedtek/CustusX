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

#ifndef CXLANDMARKREP_H_
#define CXLANDMARKREP_H_

#include "cxRepImpl.h"

#include <map>
#include <QColor>
#include "cxImage.h"
#include "vtkForwardDeclarations.h"
#include "cxGraphicalPrimitives.h"
#include "cxLandmark.h"

namespace cx
{
class View;
}

namespace cx
{
class DataManager;
class Reporter;
typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;

/**
 * \addtogroup cx_resource_visualization
 * @{
 */

class LandmarksSource: public QObject
{
Q_OBJECT
public:
	virtual ~LandmarksSource() {}
	virtual LandmarkMap getLandmarks() const = 0;
	virtual Transform3D get_rMl() const = 0;
	virtual Vector3D getTextPos(Vector3D p_l) const = 0;signals:
	void changed();
};
typedef boost::shared_ptr<LandmarksSource> LandmarksSourcePtr;

typedef boost::shared_ptr<class PatientLandmarksSource> PatientLandmarksSourcePtr;

class PatientLandmarksSource: public LandmarksSource
{
public:
	static PatientLandmarksSourcePtr New(DataServicePtr dataManager) { return PatientLandmarksSourcePtr(new PatientLandmarksSource(dataManager)); }
	PatientLandmarksSource(DataServicePtr dataManager);
	virtual ~PatientLandmarksSource() {}
	virtual LandmarkMap getLandmarks() const;
	virtual Transform3D get_rMl() const;
	virtual Vector3D getTextPos(Vector3D p_l) const;
private:
	DataServicePtr mDataManager;
};

typedef boost::shared_ptr<class ImageLandmarksSource> ImageLandmarksSourcePtr;

class ImageLandmarksSource: public LandmarksSource
{
public:
	static ImageLandmarksSourcePtr New()
	{
		return ImageLandmarksSourcePtr(new ImageLandmarksSource());
	}
	ImageLandmarksSource();
	virtual ~ImageLandmarksSource() {}
	virtual LandmarkMap getLandmarks() const;
	virtual Transform3D get_rMl() const;
	virtual Vector3D getTextPos(Vector3D p_l) const;

	void setData(DataPtr image);
	DataPtr getData();
private:
	DataPtr mData;
};

/**
 * \class LandmarkRep
 *
 * \brief
 * \ingroup cx_resource_visualization
 * \date Dec 10, 2008
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class LandmarkRep: public RepImpl
{
Q_OBJECT
public:
	static LandmarkRepPtr New(DataServicePtr dataManager, const QString& uid = "");
	virtual ~LandmarkRep();

	void setColor(QColor color); ///< sets the reps color
	void setSecondaryColor(QColor color); ///< sets the reps color
	void showLandmarks(bool on); ///< turn on or off showing landmarks
	void setGraphicsSize(double size);
	void setLabelSize(double size);
	virtual QString getType() const { return "LandmarkRep"; }

	void setPrimarySource(LandmarksSourcePtr primary);
	void setSecondarySource(LandmarksSourcePtr secondary);

protected:
	LandmarkRep(DataServicePtr dataManager); ///< sets default text scaling to 20
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
	void clearAll();
	void addAll();
	void addLandmark(QString uid);

protected slots:
	void internalUpdate(); ///< updates the text, color, scale etc

protected:
	QColor mInactiveColor; ///< color given to inactive landmarks
	QColor mColor; ///< the color of the landmark actors
	QColor mSecondaryColor; ///< color used on the secondary coordinate
	bool mShowLandmarks; ///< whether or not the actors should be showed in (all) views
	double mGraphicsSize;
	double mLabelSize;

	struct LandmarkGraphics
	{
		GraphicalLine3DPtr mLine; ///< line between primary and secondary point
		GraphicalPoint3DPtr mPrimaryPoint; ///< the primary coordinate of the landmark
		GraphicalPoint3DPtr mSecondaryPoint; ///< secondary landmark coordinate, accosiated with the primary point
		FollowerText3DPtr mText; ///< name of landmark, attached to primary point
	};
	typedef std::map<QString, LandmarkGraphics> LandmarkGraphicsMapType;
	LandmarkGraphicsMapType mGraphics;
	ViewportListenerPtr mViewportListener;
	void rescale();

	LandmarksSourcePtr mPrimary;
	LandmarksSourcePtr mSecondary;

	DataServicePtr mDataManager;

private:
	LandmarkRep(); ///< not implemented
};

/**
 * @}
 */
} //namespace cx

#endif /* CXLANDMARKREP_H_ */
