/*
 * sscVector3DDataAdapter.h
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#ifndef SSCVECTOR3DDATAADAPTER_H_
#define SSCVECTOR3DDATAADAPTER_H_


#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "sscDoubleRange.h"
#include "sscDataAdapter.h"
#include "sscVector3D.h"

namespace ssc
{


/** Abstract interface for interaction with internal Vector3D-valued data
 *
 *  Refer to DoubleDataAdapter for a description.
 *
 */
class Vector3DDataAdapter : public DataAdapter
{
  Q_OBJECT
public:
  virtual ~Vector3DDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
  virtual bool setValue(const Vector3D& value) = 0; ///< set the data value.
  virtual Vector3D getValue() const = 0; ///< get the data value.

public: // optional methods
  virtual QString getHelp() const { return QString(); } ///< return a descriptive help string for the data, used for example as a tool tip.
  virtual DoubleRange getValueRange() const { return DoubleRange(-1000,1000,0.1); } /// range of value
  virtual double convertInternal2Display(double internal) { return internal; } ///< conversion from internal value to display value (for example between 0..1 and percent)
  virtual double convertDisplay2Internal(double display) { return display; } ///< conversion from internal value to display value
  virtual int getValueDecimals() const { return 0; } ///< number of relevant decimals in value
};
typedef boost::shared_ptr<Vector3DDataAdapter> Vector3DDataAdapterPtr;


/** Dummy implementation */
class Vector3DDataAdapterNull : public Vector3DDataAdapter
{
  Q_OBJECT

public:
  virtual ~Vector3DDataAdapterNull() {}
  virtual QString getValueName() const { return "dummy"; }
  virtual bool setValue(const Vector3D& value) { return false; }
  virtual Vector3D getValue() const { return Vector3D(0,0,0); }
  virtual void connectValueSignals(bool on) {}
};

}

#endif /* SSCVECTOR3DDATAADAPTER_H_ */
