#ifndef CXVIEWGROUP_H_
#define CXVIEWGROUP_H_

#include <vector>
#include <QObject>
#include "sscData.h"
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxViewWrapper.h"
class QMenu;
class QPoint;

namespace cx
{
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

private slots:
  void contexMenuSlot(const QPoint& point);
  void activateManualToolSlot();

protected:
  std::string toString(int i) const;
  void connectContextMenu();

  std::vector<ssc::View*> mViews;

  ssc::ImagePtr mImage;
  std::vector<ViewWrapperPtr> mElements;
};

} // namespace cx

#endif /* CXVIEWGROUP_H_ */
