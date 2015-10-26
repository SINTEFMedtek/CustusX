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

#ifndef CXPLAYBACKTOOL_H_
#define CXPLAYBACKTOOL_H_

#include "cxToolImpl.h"

#include "cxResourceExport.h"

namespace cx
{

typedef boost::shared_ptr<class PlaybackTool> PlaybackToolPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;


/** \brief A tool used during playback
 *
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 *
 * \ingroup org_custusx_core_tracking
 */
class cxResource_EXPORT PlaybackTool: public ToolImpl
{
Q_OBJECT
public:
	explicit PlaybackTool(ToolPtr base, PlaybackTimePtr time);
	virtual ~PlaybackTool();
	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void setTransformSaveFile(const QString& filename) {}
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual bool isCalibrated() const;
	virtual double getTimestamp() const;

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<int, Vector3D> getReferencePoints() const;


	virtual TimedTransformMapPtr getPositionHistory() { return mBase->getPositionHistory(); }
	virtual bool isInitialized() const;
	virtual ProbePtr getProbe() const { return mBase->getProbe(); }
	virtual bool hasReferencePointWithId(int id) { return mBase->hasReferencePointWithId(id); }
	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime) { return mBase->getSessionHistory(startTime, stopTime); }

	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	virtual void setVisible(bool vis);

	// extensions
	ToolPtr getBase() { return mBase; }

private slots:
	void timeChangedSlot();
private:
	ToolPtr mBase;
	PlaybackTimePtr mTime;

	bool mVisible;
	double mTimestamp;
	Transform3D m_rMpr;
};

} /* namespace cx */
#endif /* CXPLAYBACKTOOL_H_ */
