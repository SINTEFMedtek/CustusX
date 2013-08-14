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

/**\brief DataAdapter interface for boolean values.
 *
 * \ingroup sscWidget
 */
class BoolDataAdapter: public DataAdapter
{
Q_OBJECT
public:
	virtual ~BoolDataAdapter()
	{
	}

public:
	// basic methods
	virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
	virtual bool setValue(bool value) = 0; ///< set the data value.
	virtual bool getValue() const = 0; ///< get the data value.

public:
	// optional methods
	virtual QString getHelp() const
	{
		return QString();
	} ///< return a descriptive help string for the data, used for example as a tool tip.
	//virtual void connectValueSignals(bool on) {} ///< set object to emit changed() when applicable

	//signals:
	//  void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<BoolDataAdapter> BoolDataAdapterPtr;

}

#endif /* SSCBOOLDATAADAPTER_H_ */
