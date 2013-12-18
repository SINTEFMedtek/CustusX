// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXMANUALTOOLADAPTER_H_
#define CXMANUALTOOLADAPTER_H_

#include "sscManualTool.h"

namespace cx
{
/**
* \file
* \addtogroup cxServiceTracking
* @{
*/

/**
 * \brief Adapter class for ManualTool.
 * \ingroup cxServiceTracking
 *
 * A ManualToolAdapter inherits from manual tool, but also
 * contains a cx::Tool that is requests shape and probe info from.
 *
 * Used for debug - when testing tools without a tracking system.
 *
 *  \date Feb 14, 2011
 *  \author christiana
 */
class ManualToolAdapter : public ManualTool
{
	Q_OBJECT
public:
	explicit ManualToolAdapter(ToolManager* manager, QString uid);
	explicit ManualToolAdapter(ToolManager* manager, ToolPtr base);
	virtual ~ManualToolAdapter();

	virtual std::set<Type> getTypes() const { return mBase->getTypes(); }
	virtual QString getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual bool isCalibrated() const;
	virtual ProbeDefinition getProbeSector() const;
	virtual ProbePtr getProbe() const;

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<int, Vector3D> getReferencePoints() const;

	void setBase(ToolPtr base);

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

private:
  ToolPtr mBase;
};

typedef boost::shared_ptr<ManualToolAdapter> ManualToolAdapterPtr;


/**
* @}
*/
}

#endif /* CXMANUALTOOLADAPTER_H_ */
