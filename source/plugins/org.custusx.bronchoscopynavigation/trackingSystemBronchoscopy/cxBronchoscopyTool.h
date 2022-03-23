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

#ifndef CXBRONCHOSCOPYTOOL_H_
#define CXBRONCHOSCOPYTOOL_H_

#include "cxToolImpl.h"

#include "org_custusx_bronchoscopynavigation_Export.h"

namespace cx
{

typedef boost::shared_ptr<class BronchoscopyTool> BronchoscopyToolPtr;
typedef boost::shared_ptr<class BronchoscopePositionProjection> BronchoscopePositionProjectionPtr;


/** \brief A tool used during playback
 *
 * \date 2014-11-11
 * \author Erlend Hofstad & Christian Askeland, SINTEF
 *
 * \ingroup cx_service_tracking
 */
class org_custusx_bronchoscopynavigation_EXPORT BronchoscopyTool: public ToolImpl
{
Q_OBJECT
public:
	explicit BronchoscopyTool(ToolPtr base, BronchoscopePositionProjectionPtr	projectionCenterline);
	virtual ~BronchoscopyTool();
	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual bool isCalibrated() const;
	virtual double getTimestamp() const;

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<QString, Vector3D> getReferencePoints() const;


	virtual TimedTransformMapPtr getPositionHistory() { return mBase->getPositionHistory(); }
	virtual bool isInitialized() const;
	virtual ProbePtr getProbe() const { return mBase->getProbe(); }
	virtual bool hasReferencePointWithId(QString id) { return mBase->hasReferencePointWithId(id); }
	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime) { return mBase->getSessionHistory(startTime, stopTime); }

	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	virtual void setVisible(bool vis);

	// extensions
	ToolPtr getBaseTool() { return mBase; }

private:
	ToolPtr mBase;
	Transform3D m_prMt;
	BronchoscopePositionProjectionPtr mProjectionCenterline;

private slots:
	void onToolTransformAndTimestamp(Transform3D matrix, double timestamp);


	//Transform3D m_rMpr;
};

} /* namespace cx */
#endif /* CXBRONCHOSCOPYTOOL_H_ */
