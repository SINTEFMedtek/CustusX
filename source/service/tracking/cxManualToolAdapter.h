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
 * \brief Adapter class for ssc::ManualTool.
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
class ManualToolAdapter : public ssc::ManualTool
{
	Q_OBJECT
public:
	explicit ManualToolAdapter(ssc::ToolManager* manager, QString uid);
	explicit ManualToolAdapter(ssc::ToolManager* manager, ssc::ToolPtr base);
	virtual ~ManualToolAdapter();

	virtual bool isManual()     const { return true; }
	virtual bool isReference()  const { return mBase->isReference(); }
	virtual bool isPointer()    const { return mBase->isPointer(); }
	virtual bool isProbe()      const { return mBase->isProbe(); }
	virtual bool isMicroscope() const { return mBase->isMicroscope(); }

	virtual QString getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual bool isCalibrated() const;
	virtual ssc::ProbeData getProbeSector() const;
	virtual ssc::ProbePtr getProbe() const;

	virtual ssc::Transform3D getCalibration_sMt() const;
	virtual std::map<int, ssc::Vector3D> getReferencePoints() const;

	void setBase(ssc::ToolPtr base);

private:
  ssc::ToolPtr mBase;
};

typedef boost::shared_ptr<ManualToolAdapter> ManualToolAdapterPtr;


/**
* @}
*/
}

#endif /* CXMANUALTOOLADAPTER_H_ */
