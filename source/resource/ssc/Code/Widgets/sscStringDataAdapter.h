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

#ifndef SSCSTRINGDATAADAPTER_H_
#define SSCSTRINGDATAADAPTER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QComboBox>
#include "sscDataAdapter.h"

namespace ssc
{

/**\brief Abstract interface for interaction with internal string-valued data
 *
 * The class provides a bridge between general user interface code and specific
 * data structures. An implementation connects to a single data value.
 *
 *
 * Minimal implementation:
  \verbatim
       virtual QString getValueName() const;
       virtual bool setValue(QString value);
       virtual QString getValue() const;
       void changed();
  \endverbatim
 *
 * By implementing these methods you can set and get values, and the data has a name.
 * The changed() signal is used to make sure the user interface is updated even when
 * data is changed by some other source.
 *
 *
 * For more control use the methods:
  \verbatim
       virtual QStringList getValueRange() const;
  \endverbatim
 *
 *
 * If there is a difference between the internal data representation and
 * how you want to present them, use:
 *
  \verbatim
       virtual QString convertInternal2Display(QString internal);
  \endverbatim
 *
 * When testing, or during development, you can use the StringDataAdapterNull
 * as a dummy implementation.
 *
 * \ingroup sscWidget
 */
class StringDataAdapter: public DataAdapter
{
Q_OBJECT
public:
	virtual ~StringDataAdapter()
	{
	}

public:
	// basic methods
	virtual QString getValueName() const = 0;///< name of data entity. Used for display to user.
	virtual bool setValue(const QString& value) = 0; ///< set the data value.
	virtual QString getValue() const = 0; ///< get the data value.

public:
	/*
	 * If set: setValue will always fail. Adapter is used for display only.
	 */
	virtual bool isReadOnly() const { return false; }
	/*
	 * If true: value will always be within range. setValue using other values will fail.
	 */
	virtual bool getAllowOnlyValuesInRange() const { return true; }
	// optional methods
	virtual QString getHelp() const
	{
		return QString();
	} ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual QStringList getValueRange() const
	{
		return QStringList();
	} /// range of value. Use if data is constrained to a set.
	virtual QString convertInternal2Display(QString internal)
	{
		return internal;
	} ///< conversion from internal value to display value
};
typedef boost::shared_ptr<StringDataAdapter> StringDataAdapterPtr;

/**
 * \brief Dummy implementation of the StringDataAdapter
 *
 * \author Christian Askeland, SINTEF
 */
class StringDataAdapterNull: public StringDataAdapter
{
Q_OBJECT
public:
	virtual ~StringDataAdapterNull()
	{
	}
	virtual QString getValueName() const
	{
		return "dummy";
	}
	virtual bool setValue(const QString& value)
	{
		return false;
	}
	virtual QString getValue() const
	{
		return "";
	}
};
} // namespace ssc

#endif /* SSCSTRINGDATAADAPTER_H_ */
