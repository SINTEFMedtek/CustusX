// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXLANDMARKREP_H_
#define CXLANDMARKREP_H_

#include <sscRepImpl.h>

#include <map>
#include <QColor>
#include <sscImage.h>
#include "vtkForwardDeclarations.h"
#include "sscGraphicalPrimitives.h"
#include "sscLandmark.h"

namespace cx
{
class View;
}

namespace cx
{
class MessageManager;
typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;

/**
 * \addtogroup cxServiceVisualizationRep
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
	static PatientLandmarksSourcePtr New() { return PatientLandmarksSourcePtr(new PatientLandmarksSource()); }
	PatientLandmarksSource();
	virtual ~PatientLandmarksSource() {}
	virtual LandmarkMap getLandmarks() const;
	virtual Transform3D get_rMl() const;
	virtual Vector3D getTextPos(Vector3D p_l) const;
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

	void setImage(ImagePtr image);
private:
	ImagePtr mImage;
};

/**
 * \class LandmarkRep
 *
 * \brief
 * \ingroup cxServiceVisualizationRep
 *
 * \date Dec 10, 2008
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class LandmarkRep: public RepImpl
{
Q_OBJECT
public:
	static LandmarkRepPtr New(const QString& uid, const QString& name = "");
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
	LandmarkRep(const QString& uid, const QString& name = ""); ///< sets default text scaling to 20
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);
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

private:
	LandmarkRep(); ///< not implemented
};

/**
 * @}
 */
} //namespace cx

#endif /* CXLANDMARKREP_H_ */
