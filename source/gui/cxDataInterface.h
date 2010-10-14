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
//#include <QtGui>
#include "sscForwardDeclarations.h"
#include "sscDoubleDataAdapter.h"
//#include "sscHelperWidgets.h"
#include "sscStringDataAdapter.h"

namespace cx
{

/** Interface to the tool offset of the dominant tool
 */
class DoubleDataAdapterActiveToolOffset : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterActiveToolOffset();
  virtual ~DoubleDataAdapterActiveToolOffset() {}
  virtual QString getValueName() const { return "Offset (mm)"; }
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
  ssc::DoubleRange getValueRange() const;

private slots:
  void dominantToolChangedSlot();
protected:
  ssc::ToolPtr mTool;
};

/** Superclass for all doubles interacting with the active image.
 */
class DoubleDataAdapterActiveImageBase : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterActiveImageBase();
  virtual ~DoubleDataAdapterActiveImageBase() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
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
class DoubleDataAdapter2DWindow : public DoubleDataAdapterActiveImageBase
{
public:
  virtual ~DoubleDataAdapter2DWindow() {}
  virtual QString getValueName() const { return "Window"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/**DataInterface implementation for the LUT 2D level value
 */
class DoubleDataAdapter2DLevel : public DoubleDataAdapterActiveImageBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapter2DLevel() {}
  virtual QString getValueName() const { return "Level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};


/** Adapter that connects to the current active image.
 * Example: Active image: [DataName]
 * where active image is the value
 * and DataName is taken from the valuerange
 */
class ActiveImageStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new ActiveImageStringDataAdapter()); }
  ActiveImageStringDataAdapter();
  virtual ~ActiveImageStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};

typedef boost::shared_ptr<class ParentFrameStringDataAdapter> ParentFrameStringDataAdapterPtr;

class ParentFrameStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ParentFrameStringDataAdapterPtr New() { return ParentFrameStringDataAdapterPtr(new ParentFrameStringDataAdapter()); }
  ParentFrameStringDataAdapter();
  void setData(ssc::DataPtr data);
  virtual ~ParentFrameStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

private:
  ssc::DataPtr mData;
};

} // namespace cx

#endif /* CXDATAINTERFACE_H_ */
