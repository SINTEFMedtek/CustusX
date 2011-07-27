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
  bool mInternalUpdate;
};

class PlaneMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit PlaneMetricWrapper(PlaneMetricPtr data);
  virtual ~PlaneMetricWrapper() {}
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
  PlaneMetricPtr mData;
  ssc::StringDataAdapterXmlPtr mFrameSelector;
  ssc::Vector3DDataAdapterXmlPtr mCoordinate;
  ssc::Vector3DDataAdapterXmlPtr mNormal;
  bool mInternalUpdate;
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
  std::vector<ssc::StringDataAdapterXmlPtr> mPSelector;
  void getPointMetrics(QStringList* uid, std::map<QString,QString>* namemap);
  bool mInternalUpdate;
};

class AngleMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit AngleMetricWrapper(AngleMetricPtr data);
  virtual ~AngleMetricWrapper() {}
  virtual QWidget* createWidget();
  virtual QString getValue() const;
  virtual ssc::DataPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;

private slots:
  void pointSelected();
  void dataChangedSlot();

private:
  AngleMetricPtr mData;
  std::vector<ssc::StringDataAdapterXmlPtr> mPSelector;
  void getPointMetrics(QStringList* uid, std::map<QString,QString>* namemap);
  bool mInternalUpdate;
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
  ssc::Vector3D getSample() const;
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
