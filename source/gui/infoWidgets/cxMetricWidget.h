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
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscAngleMetric.h"
#include "sscPlaneMetric.h"
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

protected:
  void paintEvent(QPaintEvent* event);
  void setModified();

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void enablebuttons();
  void setActiveUid(QString uid);
  ssc::PointMetricPtr addPoint(ssc::Vector3D point, ssc::CoordinateSystem space=ssc::CoordinateSystem(ssc::csREF), QString name="point%1");
  MetricBasePtr createMetricWrapper(ssc::DataPtr data);
  std::vector<MetricBasePtr> createMetricWrappers();
  std::vector<ssc::DataPtr> refinePointArguments(std::vector<ssc::DataPtr> args, unsigned argNo);
  void setManualToolPosition(ssc::Vector3D p_r);
  void prePaintEvent();

//  template<class T>
  QAction* createAction(QActionGroup* group, QString iconName, QString text, QString tip, const char* slot);

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks
  QString mActiveLandmark; ///< uid of surrently selected landmark.

  std::vector<MetricBasePtr> mMetrics;

  QAction* mRemoveAction; ///< the Remove Landmark button
  QAction* mLoadReferencePointsAction; ///< button for loading a reference tools reference points
  QStackedWidget* mEditWidgets;
  bool mModified; ///< use to compute only prior to paintEvent()
};

}//end namespace cx

#endif /* CXMETRICWIDGET_H_ */
