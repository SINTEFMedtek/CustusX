/*
 * cxDataMetricWrappers.h
 *
 *  \date Jul 29, 2011
 *      \author christiana
 */

#ifndef CXDATAMETRICWRAPPERS_H_
#define CXDATAMETRICWRAPPERS_H_

#include <vector>
#include <QtGui>
#include "cxForwardDeclarations.h"
#include "cxLandmark.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxAngleMetric.h"
#include "cxPlaneMetric.h"
#include "cxSphereMetric.h"
#include "cxShapedMetric.h"
#include "cxStringDataAdapterXml.h"
#include "cxVector3DDataAdapterXml.h"
#include "cxColorDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

namespace cx
{

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

typedef boost::shared_ptr<class MetricBase> MetricBasePtr;

class MetricBase : public QObject
{
  Q_OBJECT
public:
	MetricBase();
  virtual ~MetricBase() {}
  virtual QWidget* createWidget() = 0;
  virtual QString getValue() const;
  virtual DataMetricPtr getData() const = 0;
  virtual QString getArguments() const = 0;
  virtual QString getType() const = 0;
	virtual void update() = 0;
//	{
//		if (!mModified)
//			return;
//		this->repaint();
//		mModified = false;
//	}

private slots:
  void colorSelected();
protected:
//  void setModified() { mModified = true; }
//  virtual void repaint() = 0;
//  bool mModified;
  ColorDataAdapterXmlPtr mColorSelector;
  void addColorWidget(QVBoxLayout* layout);
};

/**
  *
  */
class MetricReferenceArgumentListGui : public QObject
{
	Q_OBJECT
public:
	MetricReferenceArgumentListGui();
	void setArguments(MetricReferenceArgumentListPtr arguments);
	void addWidgets(QBoxLayout* layout);
	QString getAsString() const;
	void update();
//public:
//	void argumentsChanged();
private slots:
  void pointSelected();
  void dataChangedSlot();
private:
	MetricReferenceArgumentListPtr mArguments;
	std::vector<StringDataAdapterXmlPtr> mPSelector;
	bool mInternalUpdate;
	bool mModified;
	void getAvailableArgumentMetrics(QStringList* uid, std::map<QString,QString>* namemap);
};

class PointMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit PointMetricWrapper(PointMetricPtr data);
  virtual ~PointMetricWrapper() {}
  virtual QWidget* createWidget();
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void moveToToolPosition();
  void spaceSelected();
  void coordinateChanged();
  void dataChangedSlot();
private:
  QWidget* createSampleButton(QWidget* parent) const;
  StringDataAdapterXmlPtr createSpaceSelector() const;
  Vector3DDataAdapterXmlPtr createCoordinateSelector() const;
  PointMetricPtr mData;
  StringDataAdapterXmlPtr mSpaceSelector;
  Vector3DDataAdapterXmlPtr mCoordinate;
  bool mInternalUpdate;
};

class PlaneMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit PlaneMetricWrapper(PlaneMetricPtr data);
  virtual ~PlaneMetricWrapper() {}
  virtual QWidget* createWidget();
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void dataChangedSlot();
private:
  PlaneMetricPtr mData;
  MetricReferenceArgumentListGui mArguments;
  bool mInternalUpdate;
};

class DistanceMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit DistanceMetricWrapper(DistanceMetricPtr data);
  virtual ~DistanceMetricWrapper() {}
  virtual QWidget* createWidget();
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void dataChangedSlot();

private:
  DistanceMetricPtr mData;
  bool mInternalUpdate;
  MetricReferenceArgumentListGui mArguments;
};

class AngleMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit AngleMetricWrapper(AngleMetricPtr data);
  virtual ~AngleMetricWrapper() {}
  virtual QWidget* createWidget();
//  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void dataChangedSlot();
  void guiChanged();

private:
  AngleMetricPtr mData;
  MetricReferenceArgumentListGui mArguments;
  BoolDataAdapterXmlPtr mUseSimpleVisualization;
  bool mInternalUpdate;
  BoolDataAdapterXmlPtr createUseSimpleVisualizationSelector() const;

};

class DonutMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit DonutMetricWrapper(DonutMetricPtr data);
  virtual ~DonutMetricWrapper() {}
  virtual QWidget* createWidget();
//  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void dataChangedSlot();
  void guiChanged();

private:
  DoubleDataAdapterXmlPtr createRadiusSelector() const;
  DoubleDataAdapterXmlPtr createThicknessSelector() const;
  DoubleDataAdapterXmlPtr createHeightSelector() const;
  BoolDataAdapterXmlPtr createFlatSelector() const;

  DonutMetricPtr mData;
  DoubleDataAdapterXmlPtr mRadius;
  DoubleDataAdapterXmlPtr mThickness;
  DoubleDataAdapterXmlPtr mHeight;
  BoolDataAdapterXmlPtr mFlat;
  bool mInternalUpdate;
  MetricReferenceArgumentListGui mArguments;

};

class SphereMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit SphereMetricWrapper(SphereMetricPtr data);
  virtual ~SphereMetricWrapper() {}
  virtual QWidget* createWidget();
//  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void dataChangedSlot();
  void guiChanged();

private:
  DoubleDataAdapterXmlPtr createRadiusSelector() const;

  SphereMetricPtr mData;
  DoubleDataAdapterXmlPtr mRadius;
  bool mInternalUpdate;
  MetricReferenceArgumentListGui mArguments;
};

/**
 * @}
 */
}

#endif /* CXDATAMETRICWRAPPERS_H_ */
