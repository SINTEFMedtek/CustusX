/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXMANUALTOOL_H_
#define CXMANUALTOOL_H_

#include "cxResourceExport.h"

#include <QMutex>
#include "cxToolImpl.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

/** \brief A manual tool that is unconnected to any hardware.
 *
 * Implementation of a Tool that is controlled programatically.
 * Can be used as a mouse-controlled offline tool.
 *
 * Use this tool by setting the functions setTransform() and setVisible().
 *
 * This class is Threadsafe.
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ManualTool : public ToolImpl
{
	Q_OBJECT
public:

	explicit ManualTool(const QString& uid, const QString& name ="");
	virtual ~ManualTool();
	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void setTransformSaveFile(const QString& filename){}
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual int getIndex() const;
	virtual bool isCalibrated() const;
	virtual double getTimestamp() const;
	virtual void set_prMt(const Transform3D& prMt, double timestamp=-1);

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<int, Vector3D> getReferencePoints() const { return std::map<int, Vector3D>(); }
	virtual void setVisible(bool vis);

private slots:
	void read3DCrossHairSlot(double toolTipOffset) const;
private:
	// constant data
	ProbeDefinition mSector;
	vtkSTLReaderPtr mSTLReader;
	mutable vtkCursor3DPtr mCrossHair;

	bool mVisible;
	double mTimestamp;
	void update3DCrossHair(double toolTipOffset) const;
};

typedef boost::shared_ptr<ManualTool> ManualToolPtr;

}// end namespace

#endif /*SSCMANULATOOL_H_*/
