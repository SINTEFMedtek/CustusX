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


#ifndef CXSLICEPROXY_H_
#define CXSLICEPROXY_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "cxDefinitions.h"
#include "boost/scoped_ptr.hpp"
#include "cxTransform3D.h"
#include "cxIndent.h"
#include "cxForwardDeclarations.h"

namespace cx
{
class SliceComputer;
// forward declarations
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class SliceProxyInterface> SliceProxyInterfacePtr;

/**
* \file
* \addtogroup cx_resource_core_tool
* @{
*/


class cxResource_EXPORT SliceProxyInterface : public QObject
{
	Q_OBJECT
public:
	virtual ~SliceProxyInterface() {}

	virtual Transform3D get_sMr() = 0; ///< get slice transform, i.e. the matrix sMr transforming a point p in ref to slice space.
	void printSelf(std::ostream & os, Indent indent) {}

#ifdef WIN32
	typedef Transform3D Transform3D;
#endif

signals:
	void transformChanged(Transform3D sMr); ///< emitted when transform is changed.
};

class cxResource_EXPORT SimpleSliceProxy : public SliceProxyInterface
{
	Q_OBJECT
public:
	SimpleSliceProxy() : m_sMr(Transform3D::Identity()) {}
	virtual ~SimpleSliceProxy() {}
	void set_sMr(Transform3D sMr) { m_sMr=sMr; emit transformChanged(m_sMr); }
	virtual Transform3D get_sMr() { return m_sMr; }
public:
	Transform3D m_sMr;
};

typedef boost::shared_ptr<class SimpleSliceProxy> SimpleSliceProxyPtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;

/**\brief Provides a slice matrix based on definition and tool
 *
 * Helper class that connects to a tool and generates slice
 * matrices based on it and other slice parameters.
 *
 * Used as the slicer in Sonowand.
 * Used as the slicer in CustusX.
 *
 */
class cxResource_EXPORT SliceProxy : public SliceProxyInterface
{
	Q_OBJECT
public:
	static SliceProxyPtr create(PatientModelServicePtr dataManager);
	virtual ~SliceProxy();

	void setTool(ToolPtr tool);
	void setOrientation(ORIENTATION_TYPE orientation );
	void setPlane(PLANE_TYPE plane );
	void setFollowType(FOLLOW_TYPE followType);
	void initializeFromPlane(PLANE_TYPE plane, bool useGravity, bool useViewOffset, double viewportHeight, double toolViewOffset);

	void setGravity(bool use, const Vector3D& dir);
	void setToolViewOffset(bool use, double viewportHeight, double toolViewOffset);
	void setToolViewportHeight(double viewportHeight);
	void setDefaultCenter(const Vector3D& c);
	void setAlwaysUseDefaultCenter(bool on);

	SliceComputer getComputer() const;
	void setComputer(const SliceComputer& val);

	ToolPtr getTool();
	virtual Transform3D get_sMr(); ///< get slice transform, i.e. the matrix sMr transforming a point p in ref to slice space.
	virtual void printSelf(std::ostream & os, Indent indent);

	/**
	 * \brief Return true if the active tools configured tool tip offset should be used for this slice proxy
	 */
	bool getUseTooltipOffset() { return mUseTooltipOffset; }
	/**x
	 * \brief Enable or disable usage of the tools tool tip offset for this slice proxy
	 */
	void setUseTooltipOffset(bool);

signals:
	void toolTransformAndTimestamp(Transform3D prMt, double timestamp); ///< forwarded from tool
	void toolVisible(bool visible); ///< forwarding of visible in tool

private slots:
	void clinicalApplicationChangedSlot();
	void tooltipOffsetSlot(double val);
	void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
	void toolVisibleSlot(bool visible); 
	void centerChangedSlot();
	void changed();

private:
	SliceProxy(PatientModelServicePtr dataManager);
	Transform3D getSyntheticToolPos(const Vector3D& center) const;
	void initCutplane();

	ToolPtr mTool;
	boost::scoped_ptr<SliceComputer> mCutplane;
	Vector3D mDefaultCenter; ///< use this center when no tool is available
	bool mAlwaysUseDefaultCenter; ///< use def center anyway
//	QString mName; ///< for debug
	bool mUseTooltipOffset;
	PatientModelServicePtr mDataManager;
};

/**
* @}
*/

}

#endif /*CXSLICEPROXY_H_*/
