/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXDATAMETRICWRAPPERS_H_
#define CXDATAMETRICWRAPPERS_H_

#include "cxGuiExport.h"

#include <vector>
#include <QtWidgets>

#include "cxForwardDeclarations.h"
#include "cxLandmark.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxAngleMetric.h"
#include "cxPlaneMetric.h"
#include "cxSphereMetric.h"
#include "cxRegionOfInterestMetric.h"
#include "cxShapedMetric.h"
#include "cxCustomMetric.h"
#include "cxStringProperty.h"
#include "cxVector3DProperty.h"
#include "cxColorProperty.h"
#include "cxDoubleProperty.h"
#include "cxSpaceProperty.h"
#include "cxFilePathProperty.h"
#include "cxStringListProperty.h"
#include "cxSelectDataStringProperty.h"

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

class cxGui_EXPORT MetricBase : public QObject
{
  Q_OBJECT
public:
	MetricBase(VisServicesPtr services);
	virtual ~MetricBase() {}
	virtual QWidget* createWidget() = 0;
	virtual QString getValue() const;
	virtual DataMetricPtr getData() const = 0;
	virtual QString getArguments() const = 0;
	virtual QString getType() const = 0;
	virtual void update() = 0;

private slots:
  void colorSelected();
protected:
  ColorPropertyPtr mColorSelector;
  void addColorWidget(QVBoxLayout* layout);
  QWidget* newWidget(QString objectName);
  VisServicesPtr mServices;

};

/**
  *
  */
class cxGui_EXPORT MetricReferenceArgumentListGui : public QObject
{
	Q_OBJECT
public:
	MetricReferenceArgumentListGui(VisServicesPtr services);
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
	std::vector<StringPropertyPtr> mPSelector;
	bool mInternalUpdate;
	bool mModified;
	VisServicesPtr mServices;

	void getAvailableArgumentMetrics(QStringList* uid, std::map<QString,QString>* namemap);
};

class cxGui_EXPORT PointMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit PointMetricWrapper(VisServicesPtr services, PointMetricPtr data);
  virtual ~PointMetricWrapper();
  virtual QWidget* createWidget();
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void moveToToolPosition();
  void spaceSelected();
  void coordinateChanged();
//  void dataChangedSlot();
private:
  QWidget* createSampleButton(QWidget* parent) const;
  SpacePropertyPtr createSpaceSelector() const;
  Vector3DPropertyPtr createCoordinateSelector() const;
  PointMetricPtr mData;
  SpacePropertyPtr mSpaceSelector;
  Vector3DPropertyPtr mCoordinate;
  bool mInternalUpdate;
};

class cxGui_EXPORT PlaneMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit PlaneMetricWrapper(VisServicesPtr services, PlaneMetricPtr data);
  virtual ~PlaneMetricWrapper();
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

class cxGui_EXPORT DistanceMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit DistanceMetricWrapper(VisServicesPtr services, DistanceMetricPtr data);
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

class cxGui_EXPORT AngleMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit AngleMetricWrapper(VisServicesPtr services, AngleMetricPtr data);
  virtual ~AngleMetricWrapper();
  virtual QWidget* createWidget();
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
  BoolPropertyPtr mUseSimpleVisualization;
  bool mInternalUpdate;
  BoolPropertyPtr createUseSimpleVisualizationSelector() const;

};

class cxGui_EXPORT DonutMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit DonutMetricWrapper(VisServicesPtr services, DonutMetricPtr data);
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
  DoublePropertyPtr createRadiusSelector() const;
  DoublePropertyPtr createThicknessSelector() const;
  DoublePropertyPtr createHeightSelector() const;
  BoolPropertyPtr createFlatSelector() const;

  DonutMetricPtr mData;
  DoublePropertyPtr mRadius;
  DoublePropertyPtr mThickness;
  DoublePropertyPtr mHeight;
  BoolPropertyPtr mFlat;
  bool mInternalUpdate;
  MetricReferenceArgumentListGui mArguments;
};

class cxGui_EXPORT CustomMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit CustomMetricWrapper(VisServicesPtr services, CustomMetricPtr data);
  virtual ~CustomMetricWrapper() {}
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
  StringPropertyPtr createDefineVectorUpMethodSelector() const;
  DoublePropertyPtr createOffsetFromP0() const;
  DoublePropertyPtr createOffsetFromP1() const;
  DoublePropertyPtr createRepeatDistance() const;
  BoolPropertyPtr createScaletoP1() const;
  BoolPropertyPtr createShowDistanceMarkers() const;
  DoublePropertyPtr createDistanceMarkerVisibility() const;
  StringPropertySelectDataPtr createModelSelector() const;
  BoolPropertyPtr createTranslationOnly() const;
  BoolPropertyPtr createTextureFollowTool() const;

  CustomMetricPtr mData;
  StringPropertyPtr mDefineVectorUpMethod;
  BoolPropertyPtr mScaleToP1;
  QWidget* mScaleToP1Widget;
  BoolPropertyPtr mShowDistanceMarkers;
  BoolPropertyPtr mTranslationOnly;
  BoolPropertyPtr mTextureFollowTool;
  DoublePropertyPtr mOffsetFromP0;
  DoublePropertyPtr mOffsetFromP1;
  DoublePropertyPtr mRepeatDistance;
  DoublePropertyPtr mDistanceMarkerVisibility;
  QWidget* mDistanceMarkerVisibilityWidget;
  StringPropertySelectDataPtr mModel;
  bool mInternalUpdate;
  MetricReferenceArgumentListGui mArguments;
};

class cxGui_EXPORT SphereMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit SphereMetricWrapper(VisServicesPtr services, SphereMetricPtr data);
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
  DoublePropertyPtr createRadiusSelector() const;

  SphereMetricPtr mData;
  DoublePropertyPtr mRadius;
  bool mInternalUpdate;
  MetricReferenceArgumentListGui mArguments;
};

class cxGui_EXPORT RegionOfInterestMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit RegionOfInterestMetricWrapper(VisServicesPtr services, RegionOfInterestMetricPtr data);
  virtual ~RegionOfInterestMetricWrapper() {}
  virtual QWidget* createWidget();
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void dataChangedSlot();
  void guiChanged();

private:
  StringListPropertyPtr mDataListProperty;
  BoolPropertyPtr mUseActiveTooltipProperty;
  StringPropertyPtr mMaxBoundsDataProperty;
  DoublePropertyPtr mMarginProperty;

  RegionOfInterestMetricPtr mData;
  bool mInternalUpdate;

  DoublePropertyPtr createMarginSelector() const;
  StringListPropertyPtr createDataListProperty();
  BoolPropertyPtr createUseActiveTooltipSelector() const;
  StringPropertyPtr createMaxBoundsDataSelector();
};



/**
 * @}
 */
}

#endif /* CXDATAMETRICWRAPPERS_H_ */
