/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "cxShapedMetric.h"
#include "cxCustomMetric.h"
#include "cxStringProperty.h"
#include "cxVector3DProperty.h"
#include "cxColorProperty.h"
#include "cxDoubleProperty.h"
#include "cxSpaceProperty.h"
#include "cxFilePathProperty.h"

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
	MetricBase(ViewServicePtr viewService, PatientModelServicePtr patientModelService);
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
  ViewServicePtr mViewService;
  PatientModelServicePtr mPatientModelService;
};

/**
  *
  */
class cxGui_EXPORT MetricReferenceArgumentListGui : public QObject
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
	std::vector<StringPropertyPtr> mPSelector;
	bool mInternalUpdate;
	bool mModified;
	void getAvailableArgumentMetrics(QStringList* uid, std::map<QString,QString>* namemap);
};

class cxGui_EXPORT PointMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit PointMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, PointMetricPtr data);
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
  explicit PlaneMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, PlaneMetricPtr data);
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
  explicit DistanceMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, DistanceMetricPtr data);
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
  explicit AngleMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, AngleMetricPtr data);
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
  explicit DonutMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, DonutMetricPtr data);
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
  explicit CustomMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, CustomMetricPtr data);
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
  DoublePropertyPtr createRadiusSelector() const;
  DoublePropertyPtr createThicknessSelector() const;
  DoublePropertyPtr createHeightSelector() const;
  BoolPropertyPtr createFlatSelector() const;
  FilePathPropertyPtr createSTLFileSelector() const;

  CustomMetricPtr mData;
  DoublePropertyPtr mRadius;
  DoublePropertyPtr mThickness;
  DoublePropertyPtr mHeight;
  BoolPropertyPtr mFlat;
  FilePathPropertyPtr mSTLFile;
  bool mInternalUpdate;
  MetricReferenceArgumentListGui mArguments;

};

class cxGui_EXPORT SphereMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit SphereMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, SphereMetricPtr data);
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

/**
 * @}
 */
}

#endif /* CXDATAMETRICWRAPPERS_H_ */
