#ifndef SSCSLICEPROXY_H_
#define SSCSLICEPROXY_H_

#include <QtCore>
#include "sscTool.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscIndent.h"

namespace ssc
{
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;

/**Helper class that connects to a tool and generates slice
 * matrices based on it and other slice parameters.
 * 
 * Used as the slicer in Sonowand.
 */
class SliceProxy : public QObject
{
	Q_OBJECT
public:
	static SliceProxyPtr New(const std::string& name);
	SliceProxy();
	virtual ~SliceProxy();

	void setTool(ToolPtr tool);
	void setOrientation(ORIENTATION_TYPE orientation );
	void setPlane(PLANE_TYPE plane );
	void setFollowType(FOLLOW_TYPE followType);
	void initializeFromPlane(PLANE_TYPE plane, bool useGravity, const Vector3D& gravityDir, bool useViewOffset, double viewportHeight, double toolViewOffset);
	std::string getName() const;
	
	void setGravity(bool use, const Vector3D& dir);
	void setToolViewOffset(bool use, double viewportHeight, double toolViewOffset); 
	void setToolViewportHeight(double viewportHeight);
	void setDefaultCenter(const Vector3D& c);
	
	SliceComputer getComputer() const;
	void setComputer(const SliceComputer& val);

	ToolPtr getTool();
	Transform3D get_sMr(); ///< get slice transform, i.e. the matrix sMr transforming a point p in ref to slice space.
	void printSelf(std::ostream & os, Indent indent);
	
signals:
	void transformChanged(Transform3D sMr); ///< emitted when transform is changed.
	void toolTransformAndTimestamp(Transform3D prMt, double timestamp); ///< forwarded from tool
	void toolVisible(bool visible); ///< forwarding of visible in tool
	
private slots:
	void tooltipOffsetSlot(double val);
	void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
	void toolVisibleSlot(bool visible); 
	void centerChangedSlot();
	void changed();
private:
	Transform3D getSyntheticToolPos(const Vector3D& center) const;
	ToolPtr mTool;
	SliceComputer mCutplane;
	Vector3D mDefaultCenter; ///< use this center when no tool is available
	std::string mName; ///< for debug
};

}

#endif /*SSCSLICEPROXY_H_*/
