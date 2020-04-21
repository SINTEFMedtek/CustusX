/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXGENERICSCRIPTFILTER_H
#define CXGENERICSCRIPTFILTER_H

#include "cxFilterImpl.h"

namespace cx
{

/** Generic filter calling external filter script.
 *
 *
 * \ingroup cx_resource_filter
 * \date Mar 10, 2020
 * \author Torgrim Lie
 */

class cxResourceFilter_EXPORT GenericScriptFilter : public FilterImpl
{
	Q_OBJECT

public:
	GenericScriptFilter(VisServicesPtr services);
	virtual ~GenericScriptFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	StringPropertyPtr getParameterFile(QDomElement root);
	StringPropertyPtr getScriptContent(QDomElement root);
	StringPropertyPtr setScriptOutput(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	vtkImageDataPtr mRawResult;
};
typedef boost::shared_ptr<class GenericScriptFilter> GenericScriptFilterPtr;


} // namespace cx



#endif // CXGENERICSCRIPTFILTER_H
