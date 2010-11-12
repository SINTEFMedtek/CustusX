#ifndef SSCTOOL_H_
#define SSCTOOL_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscProbeSector.h"
#include "sscIndent.h"

namespace ssc
{

/**Interface to a tool,
 * i.e. a pointer, US probe or similar.
 *
 * The tool position is defined in its own space (as given
 * by the tool's transform) as follows:
 *  - the origin is the tool tip
 * 	- the z axis points in the acting direction (us probe ray dir or pointing dir).
 *  - the y axis points to the left side of the tool.
 */
class Tool : public QObject
{
	Q_OBJECT
public:
	Tool() :
		mUid(""),
		mName("")
	{}
	 Tool(const QString& uid, const QString& name ="") :
	    mUid(uid),
	    mName(name)
	  {
	   if(name.isEmpty())
	     mName = uid;
	  };
	~Tool(){}

	/**Enumerates the general type of tool.
	 */
	enum Type
	{
		TOOL_NONE,
		TOOL_REFERENCE,
		TOOL_MANUAL, ///< representation of a mouse/keyboard-controlled virtual tool
	  TOOL_POINTER,
	  TOOL_US_PROBE
	};
	virtual Type getType() const = 0;
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
	/**Saves the tools internal buffers of transforms and timestamps to file.
	 */
	virtual void saveTransformsAndTimestamps() = 0;
	/**Which file to use when calling saveTransformsAndTimestamps().
	 */
	virtual void setTransformSaveFile(const QString& filename) = 0;
	virtual Transform3D get_prMt() const = 0; ///< \return transform from tool to patient ref space
	//virtual void set_prMt(const Transform3D& transform) = 0; ///< \return transform from tool to patient ref space
	virtual bool getVisible() const = 0; ///< \return the visibility status of the tool
	virtual QString getUid() const = 0; ///< \return an unique id for this instance
	virtual QString getName() const = 0; ///< \return a descriptive name for this instance
	//virtual int getIndex() const = 0;///<return a index ivar due to a list..

	virtual bool isCalibrated() const = 0; ///< a tool may not be calibrated, then no tracking i allowed
	virtual Transform3D getCalibration_sMt() const = 0; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
	virtual void setCalibration_sMt(ssc::Transform3D calibration){} ///< requests to use the calibration and replaces the tools calibration file
	
	virtual ProbeSector getProbeSector() const = 0; ///< additional information if the tool represents an US Probe.
	virtual double getTimestamp() const = 0; ///< latest valid timestamp for the position matrix. 0 means indeterminate (for f.ex. manual tools)
	virtual void printSelf(std::ostream &os, Indent indent) { Q_UNUSED(os); Q_UNUSED(indent); } ///< dump internal debug data
	
	virtual double getTooltipOffset() const { return 0; } ///< get a virtual offset extending from the tool tip.
	virtual void setTooltipOffset(double val) { Q_UNUSED(val); } ///< set a virtual offset extending from the tool tip.
	virtual std::map<int, Vector3D> getReferencePoints() { return std::map<int, Vector3D>(); } ///< Get the optional reference points from this tool
	virtual bool hasReferencePointWithId(int id){return false;}

signals:
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void toolVisible(bool visible);
	void tooltipOffset(double offset);
	void toolProbeSector();

protected:
	QString mUid;
	QString mName;
};
typedef boost::shared_ptr<Tool> ToolPtr;
} // namespace ssc

#endif /*SSCTOOL_H_*/
