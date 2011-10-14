/*
 * sscSlices3DRep.h
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 */

#ifndef SSCSLICES3DREP_H_
#define SSCSLICES3DREP_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"
#include "sscDefinitions.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

//---------------------------------------------------------
namespace ssc
{

typedef boost::shared_ptr<class Slices3DRep> Slices3DRepPtr;
typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/** A rep that displays several slices through a volume set in the 3D view.
 *
 */
class Slices3DRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static Slices3DRepPtr New(const QString& uid);
	virtual ~Slices3DRep();
	virtual QString getType() const	{ return "ssc::Slices3DRep"; }

	void setShaderFile(QString shaderFile);
	void setImages(std::vector<ssc::ImagePtr> images);
	void addPlane(PLANE_TYPE plane);
	void setTool(ToolPtr tool);

protected:
	Slices3DRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

private slots:
private:
	View* mView;
	std::vector<Texture3DSlicerProxyPtr> mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif /* SSCSLICES3DREP_H_ */
