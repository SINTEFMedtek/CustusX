/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDUMMYFILTER_H
#define CXDUMMYFILTER_H

#include "cxFilterImpl.h"

namespace cx
{

/** Dummy implementation of Filter
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 17, 2012
 * \author christiana
 */
class cxResourceFilter_EXPORT DummyFilter : public FilterImpl
{
	Q_OBJECT

public:
	virtual ~DummyFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	StringPropertyBasePtr getStringOption(QDomElement root);
	DoublePropertyBasePtr getDoubleOption(QDomElement root);
	BoolPropertyBasePtr getBoolOption(QDomElement root);
};

} // namespace cx


#endif // CXDUMMYFILTER_H
