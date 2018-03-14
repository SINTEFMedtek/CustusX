/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLANDMARKREP_H_
#define CXLANDMARKREP_H_

#include "cxResourceVisualizationExport.h"

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
class Reporter;
typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;

/**
 * \addtogroup cx_resource_view
 * @{
 */

class cxResourceVisualization_EXPORT LandmarksSource: public QObject
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

class cxResourceVisualization_EXPORT PatientLandmarksSource: public LandmarksSource
{
public:
	static PatientLandmarksSourcePtr New(PatientModelServicePtr dataManager) { return PatientLandmarksSourcePtr(new PatientLandmarksSource(dataManager)); }
	PatientLandmarksSource(PatientModelServicePtr dataManager);
	virtual ~PatientLandmarksSource() {}
	virtual LandmarkMap getLandmarks() const;
	virtual Transform3D get_rMl() const;
	virtual Vector3D getTextPos(Vector3D p_l) const;
private:
	PatientModelServicePtr mDataManager;
};

typedef boost::shared_ptr<class ImageLandmarksSource> ImageLandmarksSourcePtr;

class cxResourceVisualization_EXPORT ImageLandmarksSource: public LandmarksSource
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
 * \ingroup cx_resource_view
 * \date Dec 10, 2008
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT LandmarkRep: public RepImpl
{
Q_OBJECT
public:
	static LandmarkRepPtr New(PatientModelServicePtr dataManager, const QString& uid = "");
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
	LandmarkRep(PatientModelServicePtr dataManager); ///< sets default text scaling to 20
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

	PatientModelServicePtr mDataManager;

private:
	LandmarkRep(); ///< not implemented
};

/**
 * @}
 */
} //namespace cx

#endif /* CXLANDMARKREP_H_ */
