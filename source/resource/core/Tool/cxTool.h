/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXTOOL_H_
#define CXTOOL_H_

#include "cxResourceExport.h"

#include <string>
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDomNode>
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxIndent.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxProbe.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef std::map<double, Transform3D> TimedTransformMap;
typedef boost::shared_ptr<TimedTransformMap> TimedTransformMapPtr;
typedef boost::shared_ptr<class TrackingPositionFilter> TrackingPositionFilterPtr;

/**
 * Additional information describing each tool position,
 * typically device-dependent info.
 */
struct cxResource_EXPORT ToolPositionMetadata
{
	QString mData;
	QString toString() const;
};

/** \brief Interface to a tool,
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
class cxResource_EXPORT Tool: public QObject
{
	Q_OBJECT
public:
    static vtkPolyDataPtr createDefaultPolyDataCone();

	Tool(const QString& uid="", const QString& name = "");
	virtual ~Tool()	{}

	/**Enumerates the state of the tool
	 */
	enum State
	{
		tsNONE=0,         ///< not available
		tsCONFIGURED,     ///< configured with basic info
		tsINITIALIZED,    ///< connected to hardware, if any, ready to use
		tsTRACKING,       ///< emitting tracking data
	};
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

	virtual ToolPositionMetadata getMetadata() const = 0;
	virtual const std::map<double, ToolPositionMetadata>& getMetadataHistory() = 0;

	virtual std::set<Type> getTypes() const = 0;
	/**
	 * \return true is the tool has properties of the input type.
	 */
	virtual bool hasType(Type type) const
	{
		return this->getTypes().count(type);
	}
	virtual vtkPolyDataPtr getGraphicsPolyData() const = 0; ///< get geometric 3D description
	virtual TimedTransformMapPtr getPositionHistory() = 0; ///< get historical positions

	virtual bool getVisible() const = 0; ///< \return the visibility status of the tool
	virtual bool isInitialized() const	{ return true; }

	virtual QString getUid() const = 0; ///< \return an unique id for this instance
	virtual QString getName() const = 0; ///< \return a descriptive name for this instance

	virtual bool isCalibrated() const = 0; ///< a tool may not be calibrated, then no tracking is allowed
	virtual Transform3D getCalibration_sMt() const = 0; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
	virtual void setCalibration_sMt(Transform3D calibration) { Q_UNUSED(calibration); } ///< requests to use the calibration and replaces the tools calibration file

	virtual ProbePtr getProbe() const { return ProbePtr(); } ///< additional information if the tool represents an US Probe. Extends getProbeSector()
	virtual double getTimestamp() const = 0; ///< latest valid timestamp for the position matrix. 0 means indeterminate (for f.ex. manual tools)
	virtual void printSelf(std::ostream &os, Indent indent) { Q_UNUSED(os); Q_UNUSED(indent); } ///< dump internal debug data

	virtual double getTooltipOffset() const { return 0; } ///< get a virtual offset extending from the tool tip.
	virtual void setTooltipOffset(double val) { Q_UNUSED(val); } ///< set a virtual offset extending from the tool tip.
	virtual std::map<int, Vector3D> getReferencePoints() const { return std::map<int, Vector3D>(); } ///< Get the optional reference points from this tool
	virtual bool hasReferencePointWithId(int id) { Q_UNUSED(id); return false; }

	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime) = 0;
	virtual Transform3D get_prMt() const = 0;

	virtual void resetTrackingPositionFilter(TrackingPositionFilterPtr filter) = 0;

	virtual void set_prMt(const Transform3D& prMt, double timestamp=-1) = 0; ///< if available for this type, set pos, ts<0 means use current time
	virtual void setVisible(bool vis) = 0; ///< if available for this type, set visibility

	virtual bool isNull() = 0;
	static ToolPtr getNullObject();

#ifdef WIN32
	typedef Transform3D Transform3D;
#endif

signals:
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void toolVisible(bool visible);
	void tooltipOffset(double offset);
	void toolProbeSector();
	void tps(int);

protected:
	QString mUid;
	QString mName;
};
} // namespace cx

#endif /*CXTOOL_H_*/
