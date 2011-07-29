/*
 * cxMetricWidget.h
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
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

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void enablebuttons();
  PointMetricPtr addPoint(ssc::Vector3D point, ssc::CoordinateSystem frame=ssc::CoordinateSystem(ssc::csREF));
  MetricBasePtr createMetricWrapper(ssc::DataPtr data);
  std::vector<MetricBasePtr> createMetricWrappers();

  template<class T>
  QAction* createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot);

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks
  QString mActiveLandmark; ///< uid of surrently selected landmark.

  std::vector<MetricBasePtr> mMetrics;

  QPushButton* mRemoveButton; ///< the Remove Landmark button
  QPushButton* mLoadReferencePointsButton; ///< button for loading a reference tools reference points
  QStackedWidget* mEditWidgets;
};

}//end namespace cx

#endif /* CXMETRICWIDGET_H_ */
