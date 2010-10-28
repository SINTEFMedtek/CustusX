#ifndef CXLANDMARKREP_H_
#define CXLANDMARKREP_H_

#include <sscRepImpl.h>

#include <map>
#include <sscImage.h>
#include "vtkForwardDeclarations.h"

namespace ssc
{
class View;
}
namespace cx
{
class MessageManager;
typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;
typedef boost::weak_ptr<class LandmarkRep> LandmarkRepWeakPtr;

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
  // Fix for Windows
  struct RGB_
  {
    int R;
    int G;
    int B;
    RGB_() : R(0), G(255), B(0)
    {};
  }; ///< struct representing a rgb color

  static LandmarkRepPtr New(const QString& uid, const QString& name=""); ///constructor
  virtual ~LandmarkRep(); ///< empty

  virtual QString getType() const;

  void setColor(RGB_ color); ///< sets the reps color
  void setTextScale(int& x, int& y,int& z); ///< default is (20,20,20)
  void showLandmarks(bool on); ///< turn on or off showing landmarks
  void setImage(ssc::ImagePtr image); ///< sets the image data should be retrieved from
  ssc::ImagePtr getImage() const; ///< returns a pointer to the image being used

public slots:
  void landmarkAddedSlot(QString );
  void landmarkRemovedSlot(QString);
  void transformChangedSlot();

protected:
  LandmarkRep(const QString& uid, const QString& name=""); ///< sets default text scaling to 20
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);
  void addPoint(ssc::Vector3D coord, QString caption);
  void clearAll();
  void addAll();
  void setPosition(ssc::Vector3D coord, QString uid);

protected slots:
  void internalUpdate(); ///< updates the text, color, scale etc

protected:
  QString     mType;          ///< description of this reps type
  RGB_            mColor;         ///< the color of the landmark actors
  ssc::ImagePtr   mImage;         ///< the image which this rep is linked to
  bool            mShowLandmarks; ///< whether or not the actors should be showed in (all) views
  int             mTextScale[3];  ///< the textscale

  std::map<QString, vtkActorPtr>                mSkinPointActors;   ///< list of actors used to show where the point is on the skin
  std::map<QString, vtkVectorTextFollowerPair>  mTextFollowerActors; ///< list of numberactors with the text representing the number for easy updating

private:
  LandmarkRep(); ///< not implemented
};
}//namespace cx
#endif /* CXLANDMARKREP_H_ */
