/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTRINGPROPERTYCLIPPLANE_H
#define CXSTRINGPROPERTYCLIPPLANE_H

#include "cxGuiExport.h"
#include "cxStringPropertyBase.h"
#include "cxForwardDeclarations.h"
#include "cxInteractiveClipper.h"

namespace cx
{
typedef boost::shared_ptr<class StringPropertyClipPlane> StringPropertyClipPlanePtr;

/** Adapter that connects to the current active image.
 */
class cxGui_EXPORT StringPropertyClipPlane: public StringPropertyBase
{
Q_OBJECT
public:
	static StringPropertyClipPlanePtr New(InteractiveClipperPtr clipper)
	{
		return StringPropertyClipPlanePtr(new StringPropertyClipPlane(clipper));
	}
	StringPropertyClipPlane(InteractiveClipperPtr clipper);
	virtual ~StringPropertyClipPlane() {}

public:
	// basic methods
	virtual QString getDisplayName() const;
	virtual bool setValue(const QString& value);
	virtual QString getValue() const;

public:
	// optional methods
	virtual QString getHelp() const;
	virtual QStringList getValueRange() const;
	void setClipper(InteractiveClipperPtr clipper);

	InteractiveClipperPtr mInteractiveClipper;
};

}//cx

#endif // CXSTRINGPROPERTYCLIPPLANE_H
