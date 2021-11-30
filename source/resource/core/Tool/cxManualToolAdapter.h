/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMANUALTOOLADAPTER_H_
#define CXMANUALTOOLADAPTER_H_

#include "cxResourceExport.h"

#include "cxManualTool.h"

namespace cx
{

/**
 * \brief Adapter class for ManualTool.
 * \ingroup org_custusx_core_tracking
 *
 * A ManualToolAdapter inherits from manual tool, but also
 * contains a cx::Tool that is requests shape and probe info from.
 *
 * Used for debug - when testing tools without a tracking system.
 *
 *  \date Feb 14, 2011
 *  \author christiana
 */
class cxResource_EXPORT ManualToolAdapter : public ManualTool
{
	Q_OBJECT
public:
	explicit ManualToolAdapter(QString uid);
	explicit ManualToolAdapter(ToolPtr base);
	virtual ~ManualToolAdapter();

	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual bool isCalibrated() const;
	virtual ProbePtr getProbe() const;

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<QString, Vector3D> getReferencePoints() const;

	void setBase(ToolPtr base);

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	void startEmittingContinuousPositions(int msecBetweenPositions);
private slots:
	void emitPosition();
private:
	ToolPtr mBase;
};

typedef boost::shared_ptr<ManualToolAdapter> ManualToolAdapterPtr;

}

#endif /* CXMANUALTOOLADAPTER_H_ */
