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

#ifndef SSCTOOL_H_
#define SSCTOOL_H_

#include <string>
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscIndent.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscProbe.h"

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
 *
 * \ingroup sscTool
 */
class Tool: public QObject
{
	Q_OBJECT
public:
	Tool() :
		mUid(""), mName(""), mPositionHistory(new TimedTransformMap()),
		m_prMt(Transform3D::Identity())
	{
	}
	Tool(const QString& uid, const QString& name = "") :
		mUid(uid), mName(name), mPositionHistory(new TimedTransformMap()),
		m_prMt(Transform3D::Identity())
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
		TOOL_POINTER,   ///> Navigation pointer. Pointing functionality such as tool offset.
		TOOL_US_PROBE,  ///< Ultrasond probe. The tool has a Probe subinterface with a sector and a video stream.
		TOOL_MICROSCOPE ///< A tool following the focus point of a microscope
	};
	/**
	 * \return the type of the tool. Deprecated: Use getTypes() instead,
	 * in order to support multiple types.
	 */
	virtual Type getType() const
	{
		std::set<Type> types = this->getTypes();
		if (types.empty())
			return TOOL_NONE;
		return *types.begin();
	}
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
	/**\return a file containing a graphical description of the tool,
	 * if any. The file format is given by the file extension, for example
	 * usprobe_12L.stl for the SolidWorks format.
	 * \sa getGraphicsPolyData().
	 */
	virtual QString getGraphicsFileName() const = 0;
	/**Get a pointer to the tools graphical data in the form of vtkPolyData,
	 * if any. Either getGraphicsPolyData() or getGraphicsFileName() or both
	 * should return valid data. \sa getGraphicsFileName().
	 */
	virtual vtkPolyDataPtr getGraphicsPolyData() const = 0;

	//	/**Saves the tools internal buffers of transforms and timestamps to file.
	//	 */
	virtual TimedTransformMapPtr getPositionHistory() { return mPositionHistory; }
	//	virtual void saveTransformsAndTimestamps() = 0;
	//	/**Which file to use when calling saveTransformsAndTimestamps().
	//	 */
	//	virtual void setTransformSaveFile(const QString& filename) = 0;
//	virtual Transform3D get_prMt() const = 0; ///< \return transform from tool to patient ref space

	virtual bool getVisible() const = 0; ///< \return the visibility status of the tool
	virtual bool isInitialized() const	{ return true; }

	virtual QString getUid() const = 0; ///< \return an unique id for this instance
	virtual QString getName() const = 0; ///< \return a descriptive name for this instance
	//virtual int getIndex() const = 0;///<return a index ivar due to a list..

	virtual bool isCalibrated() const = 0; ///< a tool may not be calibrated, then no tracking i allowed
	virtual Transform3D getCalibration_sMt() const = 0; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
	virtual void setCalibration_sMt(Transform3D calibration) { Q_UNUSED(calibration); } ///< requests to use the calibration and replaces the tools calibration file

	virtual ProbePtr getProbe() const { return ProbePtr(); } ///< additional information if the tool represents an US Probe. Extends getProbeSector()
	virtual ProbeData getProbeSector() const = 0; ///< additional information if the tool represents an US Probe. Obsolete - use getProbe()
	virtual double getTimestamp() const = 0; ///< latest valid timestamp for the position matrix. 0 means indeterminate (for f.ex. manual tools)
	virtual void printSelf(std::ostream &os, Indent indent) { Q_UNUSED(os); Q_UNUSED(indent); } ///< dump internal debug data

	virtual double getTooltipOffset() const { return 0; } ///< get a virtual offset extending from the tool tip.
	virtual void setTooltipOffset(double val) { Q_UNUSED(val); } ///< set a virtual offset extending from the tool tip.
	virtual std::map<int, Vector3D> getReferencePoints() const { return std::map<int, Vector3D>(); } ///< Get the optional reference points from this tool
	virtual bool hasReferencePointWithId(int id) { Q_UNUSED(id); return false; }

	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime)
	{
		TimedTransformMap::iterator startIt = mPositionHistory->lower_bound(startTime);
		TimedTransformMap::iterator stopIt = mPositionHistory->upper_bound(stopTime);

		TimedTransformMap retval(startIt, stopIt);
		return retval;
	}

	virtual Transform3D get_prMt() const
	{
		return m_prMt;
	}

	virtual CoordinateSystem getSensorCoordinateSystem() { return CoordinateSystem(csSENSOR, this->getUid()); }
	virtual CoordinateSystem getToolCoordinateSystem() { return CoordinateSystem(csTOOL, this->getUid()); }
	virtual CoordinateSystem getToolOffsetCoordinateSystem() { return CoordinateSystem(csTOOL_OFFSET, this->getUid()); }

#ifdef WIN32
	typedef Transform3D Transform3D;
#endif

signals:
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void toolVisible(bool visible);
	void tooltipOffset(double offset);
	void toolProbeSector();///< Only used by sscDummyTool
	void tps(int);
	void probeChanged(); ///< Deprecated: Use sscProbe::sectorChanged() instead.  (Probe/tool characteristics changed)

protected:
	QString mUid;
	QString mName;
	TimedTransformMapPtr mPositionHistory;
	Transform3D m_prMt; ///< the transform from the tool to the patient reference

	virtual void set_prMt(const Transform3D& prMt, double timestamp)
	{
		m_prMt = prMt;
		(*mPositionHistory)[timestamp] = m_prMt;
		emit toolTransformAndTimestamp(m_prMt, timestamp);
	}
};
typedef boost::shared_ptr<Tool> ToolPtr;
} // namespace cx

#endif /*SSCTOOL_H_*/
