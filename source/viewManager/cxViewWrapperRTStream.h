#ifndef CXVIEWWRAPPERRTSTREAM_H_
#define CXVIEWWRAPPERRTSTREAM_H_

#include <vector>
#include <QtGui>
#include <QPointer>
#include "cxForwardDeclarations.h"
#include "sscData.h"
#include "sscDefinitions.h"
#include "sscMesh.h"
#include "cxViewWrapper.h"
#include "sscRTStreamRep.h"

namespace cx
{

/** Wrapper for a View that displays a RealTimeStream.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapperRTStream : public ViewWrapper
{
  Q_OBJECT
public:
  ViewWrapperRTStream(ssc::View* view);
  virtual ~ViewWrapperRTStream();
  virtual ssc::View* getView();
  virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy) {}

private slots:
  void streamLoadedSlot();
  void updateSlot();

protected:
  virtual void imageAdded(ssc::ImagePtr image) {}
  virtual void meshAdded(ssc::MeshPtr mesh) {}
  virtual void imageRemoved(const QString& uid) {}
  virtual void meshRemoved(const QString& uid) {}

private:
  virtual void appendToContextMenu(QMenu& contextMenu);
  void addReps();

  ssc::RealTimeStreamFixedPlaneRepPtr mStreamRep;
  ssc::RealTimeStreamSourcePtr mSource;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  QPointer<ssc::View> mView;
};
typedef boost::shared_ptr<ViewWrapperRTStream> ViewWrapperRTStreamPtr;

} // namespace cx

#endif /* CXVIEWWRAPPERRTSTREAM_H_ */
