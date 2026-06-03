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
#include "cxTransform3D.h"
#include "cxDoubleProperty.h"
#include "cxStringProperty.h"

namespace cx
{

/**
 * Filter that registers two meshes using the Coherent Point Drift (CPD) rigid algorithm
 * via the pycpd Python library. The computed rigid transform is applied as a registration
 * to the moving mesh (and all data in the same frame tree), so no new mesh is created.
 *
 * Input 0: Fixed/target mesh
 * Input 1: Moving/source mesh
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

	DoublePropertyPtr getMaxIterationsOption(QDomElement root);
	DoublePropertyPtr getToleranceOption(QDomElement root);
	DoublePropertyPtr getOutlierWeightOption(QDomElement root);
	StringPropertyPtr getScaleModeOption(QDomElement root);
	DoublePropertyPtr getScaleThresholdOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

	bool writeMeshPoints(vtkPolyDataPtr polyData, const QString& filePath);
	bool readTransform(const QString& filePath, Transform3D& deltaRMd);

private:
	QString findCPDScript() const;
	QString getVenvPythonPath() const;
	bool ensureVenv();

	Transform3D mDeltaRMd;
};

typedef boost::shared_ptr<class CPDFilter> CPDFilterPtr;

} // namespace cx

#endif // CXCPDFILTER_H
