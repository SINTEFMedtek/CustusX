#ifndef CXVIEWGROUPINRIA_H_
#define CXVIEWGROUPINRIA_H_

#include "cxViewGroup.h"

namespace cx
{
/**
 * \class cxViewGroupInria.h
 *
 * \brief
 *
 * \date 6. apr. 2010
 * \author: jbake
 */class ViewGroupInria : public ViewGroup
{
  Q_OBJECT
public:
  ViewGroupInria(int startIndex, ssc::View* view1,
      ssc::View* view2, ssc::View* view3);
  virtual ~ViewGroupInria();

  virtual void addImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);
  virtual void addXml(QDomNode& dataNode) {} ///< store internal state info in dataNode
  virtual void parseXml(QDomNode dataNode) {}///< load internal state info from dataNode

private slots:
  void contextMenuSlot(const QPoint& point);

protected:
  void connectContextMenu();
  void connectContextMenu(ssc::View* view);
  QString toString(int i) const;
  int mStartIndex;
  ssc::ImagePtr mImage;
};
}//namespace cx
#endif /* CXVIEWGROUPINRIA_H_ */
