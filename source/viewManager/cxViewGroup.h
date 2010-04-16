#ifndef CXVIEWGROUP_H_
#define CXVIEWGROUP_H_

#include <vector>
#include <QObject>
#include <QDomDocument>
#include "sscData.h"
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxViewWrapper.h"
class QMenu;
class QPoint;

namespace cx
{

/**Helper for functions independent of state.
 * Not sure if we need this - think of better place.
 *
 */
class Navigation
{
public:
  void centerToImageCenter();
  void centerToTooltip();
  ssc::Vector3D findGlobalImageCenter();
};

/**
 * \class cxViewGroup.h
 *
 * \brief
 *
 * \date 18. mars 2010
 * \author: jbake
 */
class ViewGroup : public QObject
{
  Q_OBJECT
public:
  ViewGroup();
  virtual ~ViewGroup();

  void addViewWrapper(ViewWrapperPtr wrapper);
  std::vector<ssc::View*> getViews() const;
  ssc::View* initializeView(int index, ssc::PLANE_TYPE plane);
  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);
  virtual void addXml(QDomNode& dataNode); ///< store internal state info in dataNode
  virtual void parseXml(QDomNode dataNode);///< load internal state info from dataNode
  double getZoom2D();

signals:
  void viewGroupZoom2DChanged(double newZoom);

public slots:
  void zoom2DChangeSlot(double newZoom);
  void orientationChangedSlot(ssc::ORIENTATION_TYPE type);
  void changeImage(QString imageUid);

private slots:
  void activateManualToolSlot();
  void activeImageChangeSlot();

protected:
  //zoom2d is the same for all viewwrapper2ds in a viewgroup
  void setZoom2D(double newZoom);

  std::vector<ssc::View*> mViews;

  ssc::ImagePtr mImage;
  std::vector<ViewWrapperPtr> mElements;
};
bool isViewWrapper2D(ViewWrapperPtr wrapper);
} // namespace cx

#endif /* CXVIEWGROUP_H_ */
