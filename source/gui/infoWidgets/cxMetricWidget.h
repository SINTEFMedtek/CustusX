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
#include "sscStringDataAdapterXml.h"
#include "sscVector3DDataAdapterXml.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

namespace cx
{

typedef boost::shared_ptr<class MetricBase> MetricBasePtr;

class MetricBase : public QObject
{
	Q_OBJECT
public:
	virtual ~MetricBase() {}
	virtual QWidget* createWidget() = 0;
	virtual QString getValue() const = 0;
	virtual ssc::DataPtr getData() const = 0;
	virtual QString getArguments() const = 0;
	virtual QString getType() const = 0;
};

class PointMetricWrapper : public MetricBase
{
	Q_OBJECT
public:
	explicit PointMetricWrapper(PointMetricPtr data);
	virtual ~PointMetricWrapper() {}
	virtual QWidget* createWidget();
	virtual QString getValue() const;
	virtual ssc::DataPtr getData() const;
  virtual QString getArguments() const;
	virtual QString getType() const;
private slots:
	void moveToToolPosition();
	void frameSelected();
	void coordinateChanged();
	void dataChangedSlot();
private:
	PointMetricPtr mData;
	ssc::StringDataAdapterXmlPtr mFrameSelector;
  ssc::Vector3DDataAdapterXmlPtr mCoordinate;
};

class DistanceMetricWrapper : public MetricBase
{
	Q_OBJECT
public:
	explicit DistanceMetricWrapper(DistanceMetricPtr data);
	virtual ~DistanceMetricWrapper() {}
	virtual QWidget* createWidget();
	virtual QString getValue() const;
	virtual ssc::DataPtr getData() const;
  virtual QString getArguments() const;
	virtual QString getType() const;

private slots:
	void pointSelected();
	void dataChangedSlot();

private:
	DistanceMetricPtr mData;
  ssc::StringDataAdapterXmlPtr mP0Selector;
  ssc::StringDataAdapterXmlPtr mP1Selector;
  void getPointMetrics(QStringList* uid, std::map<QString,QString>* namemap);
};


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

//  void addButtonClickedSlot();
//  void editButtonClickedSlot();
  void removeButtonClickedSlot();
//  void gotoButtonClickedSlot();
  void loadReferencePointsSlot();
//  void testSlot();

  void addPointButtonClickedSlot();
  void addDistanceButtonClickedSlot();
  void cellChangedSlot(int row, int col);

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void setManualTool(const ssc::Vector3D& p_r);
  ssc::Vector3D getSample() const;
  void enablebuttons();
  PointMetricPtr addPoint(ssc::Vector3D point, ssc::CoordinateSystem frame=ssc::CoordinateSystem(ssc::csREF));
  MetricBasePtr createMetricWrapper(ssc::DataPtr data);
  std::vector<MetricBasePtr> createMetricWrappers();

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks
//  typedef std::vector<ssc::Landmark> LandmarkVector;
//  LandmarkVector mSamples;
  QString mActiveLandmark; ///< uid of surrently selected landmark.

  std::vector<MetricBasePtr> mMetrics;

  QPushButton* mAddPointButton;
  QPushButton* mAddDistButton;
//  QPushButton* mAddButton; ///< the Add Landmark button
//  QPushButton* mEditButton; ///< the Edit Landmark button
  QPushButton* mRemoveButton; ///< the Remove Landmark button
//  QPushButton* mTestButton;
  QPushButton* mLoadReferencePointsButton; ///< button for loading a reference tools reference points
  QStackedWidget* mEditWidgets;
};

}//end namespace cx

#endif /* CXMETRICWIDGET_H_ */
