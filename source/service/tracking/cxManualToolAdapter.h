/*
 * cxManualToolAdapter.h
 *
 *  Created on: Feb 14, 2011
 *      Author: christiana
 */

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
 * \ingroup cxServiceTracking
 *
 * A ManualToolAdapter inherits from manual tool, but also
 * contains a cx::Tool that is requests shape and probe info from.
 *
 * Used for debug - when testing tools without a tracking system.
 *
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
