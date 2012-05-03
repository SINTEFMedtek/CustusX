/*
 * cxMetricWidget.h
 *
 *  \date Jul 5, 2011
 *      \author christiana
 */

#ifndef CXMETRICWIDGET_H_
#define CXMETRICWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include "sscForwardDeclarations.h"
#include "sscLandmark.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxAngleMetric.h"
#include "cxPlaneMetric.h"
#include "sscStringDataAdapterXml.h"
#include "sscVector3DDataAdapterXml.h"
#include "cxDataMetricWrappers.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

namespace cx
{


/**
 * \ingroup cxGUI
 *
 */
class MetricWidget : public BaseWidget
{
  Q_OBJECT

public:
  MetricWidget(QWidget* parent);
  virtual ~MetricWidget();

  virtual QString defaultWhatsThis() const;

signals:

protected slots:
	void updateSlot();
	void itemSelectionChanged();

  void removeButtonClickedSlot();
  void loadReferencePointsSlot();

  void addPointButtonClickedSlot();
  void addPlaneButtonClickedSlot();
  void addAngleButtonClickedSlot();
  void addDistanceButtonClickedSlot();
  void cellChangedSlot(int row, int col);
  virtual void cellClickedSlot(int row, int column);

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void enablebuttons();
  void setActiveUid(QString uid);
  PointMetricPtr addPoint(ssc::Vector3D point, ssc::CoordinateSystem space=ssc::CoordinateSystem(ssc::csREF), QString name="point%1");
  MetricBasePtr createMetricWrapper(ssc::DataPtr data);
  std::vector<MetricBasePtr> createMetricWrappers();
  std::vector<ssc::DataPtr> refinePointArguments(std::vector<ssc::DataPtr> args, unsigned argNo);
  void setManualToolPosition(ssc::Vector3D p_r);

//  template<class T>
  QAction* createAction(QActionGroup* group, QString iconName, QString text, QString tip, const char* slot);

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks
  QString mActiveLandmark; ///< uid of surrently selected landmark.

  std::vector<MetricBasePtr> mMetrics;

  QAction* mRemoveAction; ///< the Remove Landmark button
  QAction* mLoadReferencePointsAction; ///< button for loading a reference tools reference points
  QStackedWidget* mEditWidgets;
};

}//end namespace cx

#endif /* CXMETRICWIDGET_H_ */
