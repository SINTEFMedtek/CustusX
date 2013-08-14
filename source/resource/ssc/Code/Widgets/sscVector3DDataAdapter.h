// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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

/**\brief Abstract interface for interaction with internal Vector3D-valued data
 *
 *  Refer to DoubleDataAdapter for a description.
 *
 * \ingroup sscWidget
 */
class Vector3DDataAdapter: public DataAdapter
{
Q_OBJECT
public:
	virtual ~Vector3DDataAdapter()
	{
	}

public:
	// basic methods
	virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
	virtual bool setValue(const Vector3D& value) = 0; ///< set the data value.
	virtual Vector3D getValue() const = 0; ///< get the data value.

public:
	// optional methods
	virtual QString getHelp() const
	{
		return QString();
	} ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual DoubleRange getValueRange() const
	{
		return DoubleRange(-1000, 1000, 0.1);
	} /// range of value
	virtual double convertInternal2Display(double internal)
	{
		return internal;
	} ///< conversion from internal value to display value (for example between 0..1 and percent)
	virtual double convertDisplay2Internal(double display)
	{
		return display;
	} ///< conversion from internal value to display value
	virtual int getValueDecimals() const
	{
		return 0;
	} ///< number of relevant decimals in value
};
typedef boost::shared_ptr<Vector3DDataAdapter> Vector3DDataAdapterPtr;

/** Dummy implementation */
class Vector3DDataAdapterNull: public Vector3DDataAdapter
{
Q_OBJECT

public:
	virtual ~Vector3DDataAdapterNull()
	{
	}
	virtual QString getValueName() const
	{
		return "dummy";
	}
	virtual bool setValue(const Vector3D& value)
	{
		return false;
	}
	virtual Vector3D getValue() const
	{
		return Vector3D(0, 0, 0);
	}
	virtual void connectValueSignals(bool on)
	{
	}
};

}

#endif /* SSCVECTOR3DDATAADAPTER_H_ */
