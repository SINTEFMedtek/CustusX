/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
