/*
 * sscStringWidgets.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef SSCSTRINGWIDGETS_H_
#define SSCSTRINGWIDGETS_H_

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "sscStringDataAdapter.h"
#include "sscXmlOptionItem.h"

namespace ssc
{

/**Data interface for interacting with string data in xml form.
 * The xml data is defined by the class StringOptionItem.
 */
class StringDataAdapterXmlNode : public StringDataAdapter
{
  Q_OBJECT
public:
  explicit StringDataAdapterXmlNode(StringOptionItem element) : mElement(element) {}
  virtual ~StringDataAdapterXmlNode()  {}
  virtual QString getValueName() const { return mElement.getName(); }
  virtual QString getHelp() const { return mElement.getHelp(); }
  virtual bool setValue(const QString& value)
  {
    QString current = this->getValue();
    if (current==value)
      return false;
    mElement.setValue(value);
    emit valueWasSet();
    return true;
  }
  virtual QString getValue() const { return mElement.getValue(); }
  virtual QStringList getValueRange() const { return mElement.getRange(); }
  virtual void connectValueSignals(bool on) {}

signals:
  void valueWasSet();
private:
  StringOptionItem mElement;
};
typedef boost::shared_ptr<StringDataAdapterXmlNode> StringDataAdapterXmlNodePtr;

/**Composite widget for string selection.
 * Consists of <namelabel, combobox>.
 * Insert a subclass of ssc::StringDataAdStringDataAdaptero connect to data.
 */
class ComboGroupWidget : public QWidget
{
  Q_OBJECT
public:
  ComboGroupWidget(QWidget* parent, StringDataAdapterPtr, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void comboIndexChanged(const QString& val);
private:
  QLabel* mLabel;
  QComboBox* mCombo;
  StringDataAdapterPtr mData;
};

} // namespace ssc

#endif /* SSCSTRINGWIDGETS_H_ */
