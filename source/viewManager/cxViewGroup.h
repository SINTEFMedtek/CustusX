#ifndef CXVIEWGROUP_H_
#define CXVIEWGROUP_H_

#include <vector>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "sscData.h"
#include "sscDefinitions.h"
class QMenu;
class QPoint;

namespace cx
{

class ViewWrapper : public QObject
{
  Q_OBJECT
public:
  virtual ~ViewWrapper() {}
  virtual void initializePlane(ssc::PLANE_TYPE plane) {}
  virtual void setImage(ssc::ImagePtr image) = 0;
  virtual void removeImage(ssc::ImagePtr image) = 0;
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode) {}
  virtual ssc::View* getView() = 0;
};
typedef boost::shared_ptr<ViewWrapper> ViewWrapperPtr;


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

  void addViewWrapper(ViewWrapperPtr object);
  std::vector<ssc::View*> getViews() const;
  ssc::View* initializeView(int index, ssc::PLANE_TYPE plane);
  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);

private slots:
  void contexMenuSlot(const QPoint& point);

protected:
  std::string toString(int i) const;
  void connectContextMenu();

  std::vector<ssc::View*> mViews;

  ssc::ImagePtr mImage;
  std::vector<ViewWrapperPtr> mElements;
};


} // namespace cx

#endif /* CXVIEWGROUP_H_ */
