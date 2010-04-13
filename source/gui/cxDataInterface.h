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


namespace cx
{

class DoubleDataInterface : public QObject
{
  Q_OBJECT
public:
  virtual ~DoubleDataInterface() {}
  virtual double getValue() const = 0;
  virtual void setValue(double val) = 0;
  virtual QString getName() const = 0;
signals:
  void valueChanged();
};
typedef boost::shared_ptr<DoubleDataInterface> DoubleDataInterfacePtr;

class DoubleDataInterfaceWindow : public DoubleDataInterface
{
public:
  DoubleDataInterfaceWindow();
  virtual ~DoubleDataInterfaceWindow();
  virtual double getValue() const;
  virtual void setValue(double val);
  virtual QString getName() const;
};

class DoubleDataInterfaceLevel : public DoubleDataInterface
{
public:
  DoubleDataInterfaceLevel();
  virtual ~DoubleDataInterfaceLevel();
  virtual double getValue() const;
  virtual void setValue(double val);
  virtual QString getName() const;
};


class SliderGroup : public QWidget
{
  Q_OBJECT
public:
  SliderGroup(QWidget* parent, DoubleDataInterfacePtr dataInterface, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void textEditedSlot(const QString& text);
private:
  QSlider* mSlider;
  QLabel* mLabel;
  QLineEdit* mEdit;
  DoubleDataInterfacePtr mData;
};


} // namespace cx

#endif /* CXDATAINTERFACE_H_ */
