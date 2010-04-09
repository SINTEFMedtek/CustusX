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

private slots:
  void contexMenuSlot(const QPoint& point);
  void activateManualToolSlot();
  void zoom2DChangeSlot(double newZoom);
  void activeImageChangeSlot();

protected:
  void connectContextMenu();
  void connectContextMenu(ssc::View* view);

  std::vector<ssc::View*> mViews;

  ssc::ImagePtr mImage;
  double mZoomFactor2D;
  std::vector<ViewWrapperPtr> mElements;
};

} // namespace cx

#endif /* CXVIEWGROUP_H_ */
