/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCPDFILTER_H
#define CXCPDFILTER_H

#include "org_custusx_filter_cpd_Export.h"

#include "cxFilterImpl.h"
#include "cxMesh.h"

namespace cx
{

/**
 * Filter that registers two meshes using the Coherent Point Drift (CPD) algorithm
 * via the pycpd Python library. Requires: pip install pycpd
 *
 * Input 0: Fixed/target mesh
 * Input 1: Moving/source mesh
 * Output:  Registered moving mesh (point positions updated to match fixed)
 *
 * \ingroup cx_module_algorithm
 */
class org_custusx_filter_cpd_EXPORT CPDFilter : public FilterImpl
{
	Q_OBJECT
	Q_INTERFACES(cx::Filter)

public:
	CPDFilter(VisServicesPtr services);
	virtual ~CPDFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

	StringPropertyPtr getRegistrationTypeOption(QDomElement root);
	DoublePropertyPtr getMaxIterationsOption(QDomElement root);
	DoublePropertyPtr getToleranceOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	bool writeMeshPoints(vtkPolyDataPtr polyData, const QString& filePath);
	bool readResultPoints(vtkPolyDataPtr polyData, const QString& filePath);
	QString findCPDScript() const;

	vtkPolyDataPtr mResultPolyData;
};

typedef boost::shared_ptr<class CPDFilter> CPDFilterPtr;

} // namespace cx

#endif // CXCPDFILTER_H
