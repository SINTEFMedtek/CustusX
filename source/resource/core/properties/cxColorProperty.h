/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCOLORPROPERTY_H_
#define CXCOLORPROPERTY_H_

#include "cxResourceExport.h"

#include "cxColorPropertyBase.h"
#include "cxXmlOptionItem.h"


namespace cx
{
typedef boost::shared_ptr<class ColorProperty> ColorPropertyPtr;

/** Property for QColor values.
 *
 *
 * \ingroup cx_resource_core_properties
 * \date Nov 22, 2012
 * \author christiana
 */
class cxResource_EXPORT ColorProperty: public ColorPropertyBase
{
Q_OBJECT
public:
	virtual ~ColorProperty()
    {
    }

    /** Make sure one given option exists witin root.
     * If not present, fill inn the input defaults.
     */
	static ColorPropertyPtr initialize(const QString& uid, QString name, QString help, QColor value, QDomNode root =
        QDomNode());

public:
    // basic methods
    virtual QString getDisplayName() const; ///< name of data entity. Used for display to user.
    virtual QString getUid() const;
    virtual bool setValue(QColor value); ///< set the data value.
    virtual QColor getValue() const; ///< get the data value.

public:
    // optional methods
    virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.

signals:
    void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
    QString mName;
    QString mUid;
    QString mHelp;
    QColor mValue;
    XmlOptionItem mStore;

};

} // namespace cx


#endif // CXCOLORPROPERTY_H_
