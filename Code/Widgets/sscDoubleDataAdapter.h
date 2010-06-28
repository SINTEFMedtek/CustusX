/*
 * sscDoubleDataAdapter.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef SSCDOUBLEDATAADAPTER_H_
#define SSCDOUBLEDATAADAPTER_H_

#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "sscDoubleRange.h"
#include "sscDataAdapter.h"

namespace ssc
{


/** Abstract interface for interaction with internal double-valued data
 *
 * The class provides a bridge between general user interface code and specific
 * data structures. An implementation connects to a single data value.
 *
 *
 * Minimal implementation:
 *      virtual QString getValueName() const;
 *      virtual bool setValue(double value);
 *      virtual double getValue() const;
 *      void changed();
 * By implementing these methods you can set and get values, and the data has a name.
 * The changed() signal is used to make sure the user interface is updated even when
 * data is changed by some other source.
 *
 *
 * For more control use the methods:
 *      virtual DoubleRange getValueRange() const;
 *      virtual int getValueDecimals() const;
 *
 *
 * If there is a difference between the internal data representation and
 * how you want to present them, use:
 *      virtual double convertInternal2Display(double internal);
 *      virtual double convertDisplay2Internal(double display);
 *
 * When testing, or during development, you can use the DoubleDataAdapterNull
 * as a dummy implementation.
 *
 */
class DoubleDataAdapter : public DataAdapter
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
  virtual bool setValue(double value) = 0; ///< set the data value.
  virtual double getValue() const = 0; ///< get the data value.

public: // optional methods
  virtual QString getHelp() const { return QString(); } ///< return a descriptive help string for the data, used for example as a tool tip.
  virtual DoubleRange getValueRange() const { return DoubleRange(0,1,0.01); } /// range of value
  virtual double convertInternal2Display(double internal) { return internal; } ///< conversion from internal value to display value (for example between 0..1 and percent)
  virtual double convertDisplay2Internal(double display) { return display; } ///< conversion from internal value to display value
  virtual int getValueDecimals() const { return 0; } ///< number of relevant decimals in value

  //virtual double legendStep() const {return 0.2;} ///< step between each pos on the legend
  //virtual void connectValueSignals(bool on) {} ///< set object to emit changed() when applicable

  signals:
  void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<DoubleDataAdapter> DoubleDataAdapterPtr;


/** Dummy implementation */
class DoubleDataAdapterNull : public DoubleDataAdapter
{
  Q_OBJECT

public:
  virtual ~DoubleDataAdapterNull() {}
  virtual QString getValueName() const { return "dummy"; }
  virtual bool setValue(double value) { return false; }
  virtual double getValue() const { return 0; }
  virtual void connectValueSignals(bool on) {}
};

}


#endif /* SSCDOUBLEDATAADAPTER_H_ */
