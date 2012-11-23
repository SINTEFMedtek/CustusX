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
#ifndef SSCCOLORDATAADAPTER_H
#define SSCCOLORDATAADAPTER_H

#include <QColor>
#include "sscDataAdapter.h"


namespace ssc
{

/** DataAdapter interface for QColor
 *
 *
 * \ingroup sscWidget
 * \date Nov 22, 2012
 * \author christiana
 */
class ColorDataAdapter: public DataAdapter
{
Q_OBJECT
public:
    virtual ~ColorDataAdapter() {}

public:
    // basic methods
    virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
    virtual bool setValue(QColor value) = 0; ///< set the data value.
    virtual QColor getValue() const = 0; ///< get the data value.

public:
    // optional methods
    virtual QString getHelp() const
    {
        return QString();
    } ///< return a descriptive help string for the data, used for example as a tool tip.
};
typedef boost::shared_ptr<ColorDataAdapter> ColorDataAdapterPtr;

} // namespace ssc

#endif // SSCCOLORDATAADAPTER_H
