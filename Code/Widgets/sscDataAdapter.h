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
 * sscDataAdapter.h
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */
#ifndef SSCDATAADAPTER_H_
#define SSCDATAADAPTER_H_

#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>

/**\brief Superclass for all data adapters.
 *
 * The data adapters are an abstraction mechanism that separates
 * data values from the user interface. Data adapters for strings,
 * doubles, booleans and so on publish their value in a generic
 * manner, thus enabling us to write generic widgets for displaying
 * and changing them.
 *
 * \ingroup sscWidget
 */
class DataAdapter: public QObject
{
Q_OBJECT
public:
	virtual ~DataAdapter()
	{
	}

public:
	// basic methods
	virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
public:
	// optional methods

signals:
	void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<DataAdapter> DataAdapterPtr;

#endif /* SSCDATAADAPTER_H_ */
