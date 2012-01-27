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
#include <sscImage.h>
#include "vtkForwardDeclarations.h"
#include "sscGraphicalPrimitives.h"

namespace ssc
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
	virtual ssc::LandmarkMap getLandmarks() const = 0;
	virtual ssc::Transform3D get_rMl() const = 0;
	virtual ssc::Vector3D getTextPos(ssc::Vector3D p_l) const = 0;signals:
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
	virtual ssc::LandmarkMap getLandmarks() const;
	virtual ssc::Transform3D get_rMl() const;
	virtual ssc::Vector3D getTextPos(ssc::Vector3D p_l) const;
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
	virtual ssc::LandmarkMap getLandmarks() const;
	virtual ssc::Transform3D get_rMl() const;
	virtual ssc::Vector3D getTextPos(ssc::Vector3D p_l) const;

	void setImage(ssc::ImagePtr image);
private:
	ssc::ImagePtr mImage;
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
class LandmarkRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static LandmarkRepPtr New(const QString& uid, const QString& name = "");
	virtual ~LandmarkRep();

	void setColor(ssc::Vector3D color); ///< sets the reps color
	void setSecondaryColor(ssc::Vector3D color); ///< sets the reps color
	void showLandmarks(bool on); ///< turn on or off showing landmarks
	void setGraphicsSize(double size);
	void setLabelSize(double size);
	virtual QString getType() const { return "LandmarkRep"; }

	void setPrimarySource(LandmarksSourcePtr primary);
	void setSecondarySource(LandmarksSourcePtr secondary);

protected:
	LandmarkRep(const QString& uid, const QString& name = ""); ///< sets default text scaling to 20
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	void clearAll();
	void addAll();
	void addLandmark(QString uid);

protected slots:
	void internalUpdate(); ///< updates the text, color, scale etc

protected:
	ssc::Vector3D mColor; ///< the color of the landmark actors
	ssc::Vector3D mSecondaryColor; ///< color used on the secondary coordinate
	bool mShowLandmarks; ///< whether or not the actors should be showed in (all) views
	double mGraphicsSize;
	double mLabelSize;

	struct LandmarkGraphics
	{
		ssc::GraphicalLine3DPtr mLine; ///< line between primary and secondary point
		ssc::GraphicalPoint3DPtr mPrimaryPoint; ///< the primary coordinate of the landmark
		ssc::GraphicalPoint3DPtr mSecondaryPoint; ///< secondary landmark coordinate, accosiated with the primary point
		ssc::FollowerText3DPtr mText; ///< name of landmark, attached to primary point
	};
	typedef std::map<QString, LandmarkGraphics> LandmarkGraphicsMapType;
	LandmarkGraphicsMapType mGraphics;
	ssc::ViewportListenerPtr mViewportListener;
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
