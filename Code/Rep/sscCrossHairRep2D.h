/*
 * vmCrossHairRep2D.h
 *
 *  Created on: Jan 13, 2009
 *      Author: christiana
 */

#ifndef SSCCROSSHAIRREP2D_H_
#define SSCCROSSHAIRREP2D_H_

//#include <vtkSmartPointer.h>
#include "sscRepImpl.h"
#include "sscTransform3D.h"

namespace ssc
{
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class CrossHair2D> CrossHair2DPtr;

typedef boost::shared_ptr<class CrossHairRep2D> CrossHairRep2DPtr;

/**Simple toolrep2D that displays a crosshair in the tool tip pos.
 */
class CrossHairRep2D : public ssc::RepImpl
{
	Q_OBJECT
public:
	static CrossHairRep2DPtr New(const QString& uid, const QString& name="");
	virtual ~CrossHairRep2D();
	virtual QString getType() const;

	void setSliceProxy(ssc::SliceProxyPtr slicer);
	void set_vpMs(const Transform3D& vpMs);

private slots:
	void sliceTransformChangedSlot(Transform3D sMr); 
	void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
	void toolVisibleSlot(bool visible); 

protected:
	CrossHairRep2D(const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

private:
	void update();
	CrossHair2DPtr mCursor;
	SliceProxyPtr mSlicer;
	Transform3D m_vpMs;
};


} // namespace vm

#endif /* SSCCROSSHAIRREP2D_H_ */
