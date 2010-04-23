/*
 * cxDataInterface.h
 *
 *  Created on: Apr 13, 2010
 *      Author: christiana
 */
#ifndef CXDATAINTERFACE_H_
#define CXDATAINTERFACE_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include <QtGui>
#include "sscForwardDeclarations.h"
#include "sscAbstractInterface.h"
#include "sscHelperWidgets.h"

namespace cx
{

/** Interface to the tool offset of the dominant tool
 */
class DoubleDataInterfaceActiveToolOffset : public ssc::DoubleDataInterface
{
  Q_OBJECT
public:
  DoubleDataInterfaceActiveToolOffset();
  virtual ~DoubleDataInterfaceActiveToolOffset() {}
  virtual QString getValueName() const { return "Offset (mm)"; }
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual QString getValueID() const { return ""; }
  virtual void connectValueSignals(bool on) {}
  ssc::DoubleRange getValueRange() const;

private slots:
  void dominantToolChangedSlot();
protected:
  ssc::ToolPtr mTool;
};


/** Superclass for all doubles interacting with the active image.
 */
class DoubleDataInterfaceActiveImageBase : public ssc::DoubleDataInterface
{
  Q_OBJECT
public:
  DoubleDataInterfaceActiveImageBase();
  virtual ~DoubleDataInterfaceActiveImageBase() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual QString getValueID() const { return ""; }
  virtual void connectValueSignals(bool on) {}
private slots:
  void activeImageChanged();
protected:
  virtual double getValueInternal() const = 0;
  virtual void setValueInternal(double val) = 0;

  ssc::ImagePtr mImage;
};

/**DataInterface implementation for the LUT 2D window value
 */
class DoubleDataInterface2DWindow : public DoubleDataInterfaceActiveImageBase
{
public:
  virtual ~DoubleDataInterface2DWindow() {}
  virtual QString getValueName() const { return "Window"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/**DataInterface implementation for the LUT 2D level value
 */
class DoubleDataInterface2DLevel : public DoubleDataInterfaceActiveImageBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataInterface2DLevel() {}
  virtual QString getValueName() const { return "Level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/**Composite widget for scalar data manipulation.
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of ssc::DoubleDataInterfacePtr in order to connect to data.
 */
class SliderGroupWidget : public QWidget
{
  Q_OBJECT
public:
  SliderGroupWidget(QWidget* parent, ssc::DoubleDataInterfacePtr dataInterface, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void textEditedSlot(const QString& text);
  void doubleValueChanged(double val);

private:
  ssc::DoubleSlider* mSlider;
  QLabel* mLabel;
  ssc::DoubleLineEdit* mEdit;
  ssc::DoubleDataInterfacePtr mData;
};


} // namespace cx

#endif /* CXDATAINTERFACE_H_ */
