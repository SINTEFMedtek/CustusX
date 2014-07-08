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

#ifndef CXTOOL_H_
#define CXTOOL_H_

#include <string>
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxIndent.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxProbe.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef std::map<double, Transform3D> TimedTransformMap;
typedef boost::shared_ptr<TimedTransformMap> TimedTransformMapPtr;

/**\brief Interface to a tool,
 * i.e. a pointer, US probe or similar.
 *
 * The tool position is defined in its own space (as given
 * by the tool's transform) as follows:
 *  - the origin is the tool tip
 * 	- the z axis points in the acting direction (us probe ray dir or pointing dir).
 *  - the y axis points to the left side of the tool.
 *  - This means that in the case of an optical tracking frame, the tracking spheres points in the negative x axis
 *
 * \ingroup cx_resource_core_tool
 */
class Tool: public QObject
{
	Q_OBJECT
public:
	Tool(const QString& uid="", const QString& name = "") :
		mUid(uid), mName(name)
	{
		if (name.isEmpty())
			mName = uid;
	}

	virtual ~Tool()
	{
	}

	/**Enumerates the general type of tool.
	 */
	enum Type
	{
		TOOL_NONE,
		TOOL_REFERENCE, ///< Reference tool.
		TOOL_MANUAL,    ///< Representation of a mouse/keyboard-controlled virtual tool
		TOOL_POINTER,   ///< Navigation pointer. Pointing functionality such as tool offset.
		TOOL_US_PROBE,  ///< Ultrasond probe. The tool has a Probe subinterface with a sector and a video stream.
		TOOL_MICROSCOPE ///< A tool following the focus point of a microscope
	};
	/**
	 * \return the types of the tool.
	 * Implement this one instead of the deprecated getType()
	 */
	virtual std::set<Type> getTypes() const = 0;
	/**
	 * \return true is the tool has properties of the input type.
	 */
	virtual bool hasType(Type type) const
	{
		return this->getTypes().count(type);
	}
	/**Get a pointer to the tools graphical data in the form of vtkPolyData.
	 */
	virtual vtkPolyDataPtr getGraphicsPolyData() const = 0;

	/**Saves the tools internal buffers of transforms and timestamps to file.
	 */
	virtual TimedTransformMapPtr getPositionHistory() = 0;

	virtual bool getVisible() const = 0; ///< \return the visibility status of the tool
	virtual bool isInitialized() const	{ return true; }

	virtual QString getUid() const = 0; ///< \return an unique id for this instance
	virtual QString getName() const = 0; ///< \return a descriptive name for this instance

	virtual bool isCalibrated() const = 0; ///< a tool may not be calibrated, then no tracking i allowed
	virtual Transform3D getCalibration_sMt() const = 0; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
	virtual void setCalibration_sMt(Transform3D calibration) { Q_UNUSED(calibration); } ///< requests to use the calibration and replaces the tools calibration file

	virtual ProbePtr getProbe() const { return ProbePtr(); } ///< additional information if the tool represents an US Probe. Extends getProbeSector()
	virtual ProbeDefinition getProbeSector() const = 0; ///< additional information if the tool represents an US Probe. Obsolete - use getProbe()
	virtual double getTimestamp() const = 0; ///< latest valid timestamp for the position matrix. 0 means indeterminate (for f.ex. manual tools)
	virtual void printSelf(std::ostream &os, Indent indent) { Q_UNUSED(os); Q_UNUSED(indent); } ///< dump internal debug data

	virtual double getTooltipOffset() const { return 0; } ///< get a virtual offset extending from the tool tip.
	virtual void setTooltipOffset(double val) { Q_UNUSED(val); } ///< set a virtual offset extending from the tool tip.
	virtual std::map<int, Vector3D> getReferencePoints() const { return std::map<int, Vector3D>(); } ///< Get the optional reference points from this tool
	virtual bool hasReferencePointWithId(int id) { Q_UNUSED(id); return false; }

	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime) = 0;
	virtual Transform3D get_prMt() const = 0;

//	virtual CoordinateSystem getSensorCoordinateSystem() { return CoordinateSystem(csSENSOR, this->getUid()); }
//	virtual CoordinateSystem getToolCoordinateSystem() { return CoordinateSystem(csTOOL, this->getUid()); }
//	virtual CoordinateSystem getToolOffsetCoordinateSystem() { return CoordinateSystem(csTOOL_OFFSET, this->getUid()); }

#ifdef WIN32
	typedef Transform3D Transform3D;
#endif

signals:
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void toolVisible(bool visible);
	void tooltipOffset(double offset);
	void toolProbeSector();///< Only used by sscDummyTool
	void tps(int);
//	void probeChanged(); ///< Deprecated: Use sscProbe::sectorChanged() instead.  (Probe/tool characteristics changed)

protected:
	QString mUid;
	QString mName;
};
typedef boost::shared_ptr<Tool> ToolPtr;
} // namespace cx

#endif /*CXTOOL_H_*/
