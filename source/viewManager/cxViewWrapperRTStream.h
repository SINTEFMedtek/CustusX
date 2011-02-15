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
 *  The view displays either a raw rt source or a us probe, depending on
 *  whats available.
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
  void updateSlot();
//  void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
  void showSectorActionSlot(bool checked);
  void probeChangedSlot();
  void configureSlot();

protected:
  virtual void imageAdded(ssc::ImagePtr image) {}
  virtual void meshAdded(ssc::MeshPtr mesh) {}
  virtual void imageRemoved(const QString& uid) {}
  virtual void meshRemoved(const QString& uid) {}

private:
  void loadStream();
  virtual void appendToContextMenu(QMenu& contextMenu);
  void addReps();
  ssc::ToolPtr getProbe();
  void setupRep(ssc::RTSourcePtr source, ssc::ToolPtr tool);

  ssc::RealTimeStreamFixedPlaneRepPtr mStreamRep;
  ssc::RTSourcePtr mSource;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::DisplayTextRepPtr mDataNameText;
  QPointer<ssc::View> mView;
  ssc::ToolPtr mTool;
};
typedef boost::shared_ptr<ViewWrapperRTStream> ViewWrapperRTStreamPtr;

} // namespace cx

#endif /* CXVIEWWRAPPERRTSTREAM_H_ */
