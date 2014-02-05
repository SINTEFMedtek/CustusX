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
#include "sscForwardDeclarations.h"
#include "sscLandmark.h"
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscAngleMetric.h"
#include "sscPlaneMetric.h"
#include "sscStringDataAdapterXml.h"
#include "sscVector3DDataAdapterXml.h"
#include "sscColorDataAdapterXml.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

namespace cx
{

/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

typedef boost::shared_ptr<class MetricBase> MetricBasePtr;

class MetricBase : public QObject
{
  Q_OBJECT
public:
  virtual ~MetricBase() {}
  virtual QWidget* createWidget() = 0;
  virtual QString getValue() const = 0;
  virtual DataMetricPtr getData() const = 0;
  virtual QString getArguments() const = 0;
  virtual QString getType() const = 0;

private slots:
  void colorSelected();
protected:
  ColorDataAdapterXmlPtr mColorSelector;
  void addColorWidget(QVBoxLayout* layout);
};

class PointMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit PointMetricWrapper(PointMetricPtr data);
  virtual ~PointMetricWrapper() {}
  virtual QWidget* createWidget();
  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
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
  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
private slots:
  void moveToToolPosition();
  void spaceSelected();
  void coordinateChanged();
  void dataChangedSlot();
private:
  PlaneMetricPtr mData;
  StringDataAdapterXmlPtr mSpaceSelector;
  Vector3DDataAdapterXmlPtr mCoordinate;
  Vector3DDataAdapterXmlPtr mNormal;
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
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;

private slots:
  void pointSelected();
  void dataChangedSlot();

private:
  DistanceMetricPtr mData;
  std::vector<StringDataAdapterXmlPtr> mPSelector;
void getAvailableArgumentMetrics(QStringList* uid, std::map<QString,QString>* namemap);
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
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;

private slots:
  void pointSelected();
  void dataChangedSlot();

private:
  AngleMetricPtr mData;
  std::vector<StringDataAdapterXmlPtr> mPSelector;
  void getPointMetrics(QStringList* uid, std::map<QString,QString>* namemap);
  bool mInternalUpdate;
};


/**
 * @}
 */
}

#endif /* CXDATAMETRICWRAPPERS_H_ */
