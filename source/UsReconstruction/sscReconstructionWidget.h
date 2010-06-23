/*
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTIONWIDGET_H_
#define SSCRECONSTRUCTIONWIDGET_H_

#include <QtGui>
#include "sscReconstructer.h"
#include "sscAbstractInterface.h"
#include "sscHelperWidgets.h"
#include "sscXmlOptionItem.h"

namespace ssc
{

/** Abstract interface for interaction with internal string-valued data */
class StringDataInterface : public QObject
{
  Q_OBJECT

public:
  virtual ~StringDataInterface() {}
  virtual QString getValueID() const = 0;
  virtual QString getValueName() const = 0;
  virtual QString getHelp() const { return QString(); }
  virtual bool setValue(const QString& value) = 0; ///< implemented by subclasses to set value for this entity
  virtual QString getValue() const = 0; ///< implemented by subclasses to get value for this entity
  virtual QStringList getValueRange() const { return QStringList(); } /// range of value
  virtual QString convertInternal2Display(QString internal) { return internal; } ///< conversion from internal value to display value
//  virtual double convertDisplay2Internal(double display) { return display; } ///< conversion from internal value to display value
  virtual void connectValueSignals(bool on) = 0; ///< set object to emit changed() when applicable
signals:
  void changed();
};
typedef boost::shared_ptr<StringDataInterface> StringDataInterfacePtr;

/** dummy implementation */
class StringDataInterfaceNull : public StringDataInterface
{
  Q_OBJECT
public:
  virtual ~StringDataInterfaceNull() {}
  virtual QString getValueID() const { return ""; }
  virtual QString getValueName() const { return "dummy"; }
  virtual bool setValue(const QString& value) { return false; }
  virtual QString getValue() const { return ""; }
  virtual void connectValueSignals(bool on) {}
};

/**Data interface for interacting with string data in xml form.
 * The xml data is defined by the class StringOptionItem.
 */
class StringDataInterfaceXmlNode : public StringDataInterface
{
  Q_OBJECT
public:
  explicit StringDataInterfaceXmlNode(StringOptionItem element) : mElement(element) {}
  virtual ~StringDataInterfaceXmlNode()  {}
  virtual QString getValueID() const { return mElement.getId(); }
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
typedef boost::shared_ptr<StringDataInterfaceXmlNode> StringDataInterfaceXmlNodePtr;


/**Composite widget for string selection.
 * Consists of <namelabel, combobox>.
 * Insert a subclass of ssc::StringDataInterfacePtr in order to connect to data.
 */
class ComboGroupWidget : public QWidget
{
  Q_OBJECT
public:
  ComboGroupWidget(QWidget* parent, StringDataInterfacePtr dataInterface, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void comboIndexChanged(const QString& val);
private:
  QLabel* mLabel;
  QComboBox* mCombo;
  StringDataInterfacePtr mData;
};


class ReconstructionWidget : public QWidget
{
  Q_OBJECT
public:
  ReconstructionWidget(QWidget* parent, QString appDataPath);
  ReconstructerPtr reconstructer() {  return mReconstructer; }
  void selectData(QString inputfile);

public slots:
  void reconstruct();
  void selectData();
  void reload();
  void currentDataComboIndexChanged(const QString& text);
  void paramsChangedSlot();

private:
    ReconstructerPtr mReconstructer;

    QString mInputFile;

    QComboBox* mDataComboBox;
    QToolButton* mSelectDataButton;
    QPushButton* mReconstructButton;
    QPushButton* mReloadButton;
    QAction* mSelectDataAction;
    QLineEdit* mExtentLineEdit;
    QLineEdit* mInputSpacingLineEdit;
    ssc::SliderGroupWidget* mMaxVolSizeWidget;
    ssc::SliderGroupWidget* mSpacingWidget;
    ssc::SliderGroupWidget* mDimXWidget;
    ssc::SliderGroupWidget* mDimYWidget;
    ssc::SliderGroupWidget* mDimZWidget;

    ssc::StringDataInterfacePtr generateStringDataInterface(QString uid);
    QString getCurrentPath();
    void updateComboBox();
    void setInputFile(const QString& inputFile);
};

}//namespace
#endif //SSCRECONSTRUCTIONWIDGET_H_
