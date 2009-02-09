#ifndef SSCSLICEPROXY_H_
#define SSCSLICEPROXY_H_

#include <QtCore>
#include "sscTool.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"

namespace ssc
{

/**helper class that connects to a tool and generates slice
 * matrices based on it and other slice parameters.
 */
class SliceProxy : public QObject
{
	Q_OBJECT
public:
	SliceProxy();
	virtual ~SliceProxy();

	void setTool(ToolPtr tool);
	void setOrientation(ORIENTATION_TYPE orientation );
	void setPlane(PLANE_TYPE plane );
	void setFollowType(FOLLOW_TYPE followType);
	
	void setGravity(bool use, const Vector3D& dir);
	void setToolViewOffset(bool use, double viewportHeight, double toolViewOffset); 
	void setToolViewportHeight(double viewportHeight);
	
	ToolPtr getTool();
	Transform3D get_sMr(); ///< get slice transform, i.e. the matrix sMr transforming a point p in ref to slice space.
	
signals:
	void transformChanged(Transform3D sMr); ///< emitted when transform is changed.
	void toolTransformAndTimestamp(Transform3D prMt, double timestamp); ///< forwarded from tool
	void toolVisible(bool visible); ///< forwarding of visible in tool
private slots:
	void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
	void toolVisibleSlot(bool visible); 
	void centerChangedSlot();
private:
	Transform3D getSyntheticToolPos(const Vector3D& center) const;
	void changed();
	ToolPtr mTool;
	SliceComputer mCutplane;
};
typedef boost::shared_ptr<SliceProxy> SliceProxyPtr;

}

#endif /*SSCSLICEPROXY_H_*/
