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

  virtual QString getType() const = 0;

  void setColor(ssc::Vector3D color); ///< sets the reps color
  void showLandmarks(bool on); ///< turn on or off showing landmarks

public slots:
  virtual void landmarkAddedSlot(QString);
  void landmarkRemovedSlot(QString);
  virtual void transformChangedSlot();

protected:
  LandmarkRep(const QString& uid, const QString& name=""); ///< sets default text scaling to 20
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);
  void addPoint(QString uid);
  virtual void clearAll();
  virtual void addAll() = 0;
  virtual void setPosition(QString uid) = 0;

  void setShowLabel(bool on) { mShowLabel = on; }
  void setShowLine(bool on) { mShowLine = on; }

protected slots:
  void internalUpdate(); ///< updates the text, color, scale etc

protected:
  QString         mType;          ///< description of this reps type
  ssc::Vector3D   mColor;         ///< the color of the landmark actors
  bool            mShowLandmarks; ///< whether or not the actors should be showed in (all) views
  int             mTextScale[3];  ///< the textscale
  bool mShowLabel; ///< show text label
  bool mShowLine;  //< show line from master to target point

  struct LandmarkGraphics
  {
    ssc::GraphicalLine3DPtr mLine;
    ssc::GraphicalPoint3DPtr mPoint;
    ssc::FollowerText3DPtr mText;
  };
  typedef std::map<QString, LandmarkGraphics> LandmarkGraphicsMapType;
  LandmarkGraphicsMapType mGraphics;

private:
  LandmarkRep(); ///< not implemented
};
}//namespace cx
#endif /* CXLANDMARKREP_H_ */
