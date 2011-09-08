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

class LandmarksSource : public QObject
{
	Q_OBJECT
public:
	virtual ~LandmarksSource() {}
	virtual ssc::LandmarkMap getLandmarks() const = 0;
	virtual ssc::Transform3D get_rMl() const = 0;
signals:
	void changed();
};
typedef boost::shared_ptr<LandmarksSource> LandmarksSourcePtr;

class PatientLandmarksSource : public LandmarksSource
{
public:
	PatientLandmarksSource();
	virtual ~PatientLandmarksSource() {}
	virtual ssc::LandmarkMap getLandmarks() const;
	virtual ssc::Transform3D get_rMl() const;
};
typedef boost::shared_ptr<PatientLandmarksSource> PatientLandmarksSourcePtr;

class ImageLandmarksSource : public LandmarksSource
{
public:
	ImageLandmarksSource();
	virtual ~ImageLandmarksSource() {}
	virtual ssc::LandmarkMap getLandmarks() const;
	virtual ssc::Transform3D get_rMl() const;

	void setImage(ssc::ImagePtr image);
private:
	ssc::ImagePtr mImage;
};
typedef boost::shared_ptr<ImageLandmarksSource> ImageLandmarksSourcePtr;




/**
 * \class LandmarkRep
 *
 * \brief
 *
 * \date Dec 10, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class LandmarkRep : public ssc::RepImpl
{
  Q_OBJECT
public:
  virtual ~LandmarkRep();

//  virtual QString getType() const = 0;

  void setColor(ssc::Vector3D color); ///< sets the reps color
  void setSecondaryColor(ssc::Vector3D color); ///< sets the reps color
  void showLandmarks(bool on); ///< turn on or off showing landmarks
  void setGraphicsSize(double size);
  void setLabelSize(double size);

public slots:
////  virtual void landmarkAddedSlot(QString);
////  void landmarkRemovedSlot(QString);
//  virtual void transformChangedSlot();

protected:
	void setPrimarySource(LandmarksSourcePtr primary);
	void setSecondarySource(LandmarksSourcePtr secondary);

	LandmarkRep(const QString& uid, const QString& name=""); ///< sets default text scaling to 20
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);
//  void addPoint(QString uid);
  void clearAll();
  void addAll();
//  virtual void setPosition(QString uid) = 0;

//  void setShowLabel(bool on) { mShowLabel = on; }
//  void setShowLine(bool on) { mShowLine = on; }
  void addLandmark(QString uid);

protected slots:
  void internalUpdate(); ///< updates the text, color, scale etc

protected:
//  QString         mType;          ///< description of this reps type
  ssc::Vector3D   mColor;         ///< the color of the landmark actors
  ssc::Vector3D   mSecondaryColor; ///< color used on the secondary coordinate
  bool            mShowLandmarks; ///< whether or not the actors should be showed in (all) views
//  bool mShowLabel; ///< show text label
//  bool mShowLine;  //< show line from master to target point
  double mGraphicsSize;
  double mLabelSize;

  struct LandmarkGraphics
  {
    ssc::GraphicalLine3DPtr mLine; ///< line between primary and secondary point
//    ssc::GraphicalPoint3DPtr mPoint;
    ssc::GraphicalPoint3DPtr mPrimaryPoint; ///< the primary coordinate of the landmark
    ssc::GraphicalPoint3DPtr mSecondaryPoint; ///< secondary landmark coordinate, accosiated with the primary point
    ssc::FollowerText3DPtr mText; ///< name of landmark, attached to primary point
  };
  typedef std::map<QString, LandmarkGraphics> LandmarkGraphicsMapType;
  LandmarkGraphicsMapType mGraphics;
  ssc::ViewportListenerPtr mViewportListener;
  void rescale();
//  virtual bool exists(QString uid) const = 0;

  LandmarksSourcePtr mPrimary;
  LandmarksSourcePtr mSecondary;

private:
  LandmarkRep(); ///< not implemented
};
}//namespace cx
#endif /* CXLANDMARKREP_H_ */
