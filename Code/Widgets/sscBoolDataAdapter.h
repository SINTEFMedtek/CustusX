/*
 * sscBoolDataAdapter.h
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#ifndef SSCBOOLDATAADAPTER_H_
#define SSCBOOLDATAADAPTER_H_

#include "sscDataAdapter.h"

namespace ssc
{


class BoolDataAdapter : public DataAdapter
{
  Q_OBJECT
public:
  virtual ~BoolDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
  virtual bool setValue(bool value) = 0; ///< set the data value.
  virtual bool getValue() const = 0; ///< get the data value.

public: // optional methods
  virtual QString getHelp() const { return QString(); } ///< return a descriptive help string for the data, used for example as a tool tip.
  //virtual void connectValueSignals(bool on) {} ///< set object to emit changed() when applicable

signals:
  void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<BoolDataAdapter> BoolDataAdapterPtr;

}

#endif /* SSCBOOLDATAADAPTER_H_ */
