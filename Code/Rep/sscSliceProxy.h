// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCSLICEPROXY_H_
#define SSCSLICEPROXY_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "sscDefinitions.h"
#include "boost/scoped_ptr.hpp"
#include "sscTransform3D.h"
#include "sscIndent.h"

namespace ssc
{
class SliceComputer;
// forward declarations
typedef boost::shared_ptr<class Tool> ToolPtr;

typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;

/**\brief Provides a slice matrix based on definition and tool
 *
 * Helper class that connects to a tool and generates slice
 * matrices based on it and other slice parameters.
 *
 * Used as the slicer in Sonowand.
 * Used as the slicer in CustusX.
 *
 * \ingroup sscProxy
 */
class SliceProxy : public QObject
{
	Q_OBJECT
public:
	static SliceProxyPtr New(const QString& name);
	SliceProxy();
	virtual ~SliceProxy();

	void setTool(ToolPtr tool);
	void setOrientation(ORIENTATION_TYPE orientation );
	void setPlane(PLANE_TYPE plane );
	void setFollowType(FOLLOW_TYPE followType);
	void initializeFromPlane(PLANE_TYPE plane, bool useGravity, const Vector3D& gravityDir, bool useViewOffset, double viewportHeight, double toolViewOffset);
	QString getName() const;

	void setGravity(bool use, const Vector3D& dir);
	void setToolViewOffset(bool use, double viewportHeight, double toolViewOffset);
	void setToolViewportHeight(double viewportHeight);
	void setDefaultCenter(const Vector3D& c);
	void setAlwaysUseDefaultCenter(bool on);

	SliceComputer getComputer() const;
	void setComputer(const SliceComputer& val);

	ToolPtr getTool();
	Transform3D get_sMr(); ///< get slice transform, i.e. the matrix sMr transforming a point p in ref to slice space.
	void printSelf(std::ostream & os, Indent indent);

	/**
	 * \brief Return true if the active tools configured tool tip offset should be used for this slice proxy
	 */
	bool getUseTooltipOffset() { return mUseTooltipOffset; }
	/**x
	 * \brief Enable or disable usage of the tools tool tip offset for this slice proxy
	 */
	void setUseTooltipOffset(bool);

#ifdef WIN32
	typedef ssc::Transform3D Transform3D; 
#endif

signals:
	void transformChanged(Transform3D sMr); ///< emitted when transform is changed.
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
	Transform3D getSyntheticToolPos(const Vector3D& center) const;
	ToolPtr mTool;
	boost::scoped_ptr<SliceComputer> mCutplane;
	Vector3D mDefaultCenter; ///< use this center when no tool is available
	bool mAlwaysUseDefaultCenter; ///< use def center anyway
	QString mName; ///< for debug
	bool mUseTooltipOffset;
};

}

#endif /*SSCSLICEPROXY_H_*/
