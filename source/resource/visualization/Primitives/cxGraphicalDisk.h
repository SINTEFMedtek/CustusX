// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXGRAPHICALDISK_H
#define CXGRAPHICALDISK_H

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "sscVector3D.h"
#include <QColor>

namespace cx
{

/** Render a flat disk in 2D/3D
 *
 *
 * \ingroup cx
 * \date 2014-02-25
 * \author christiana
 */
class GraphicalDisk
{
public:
	GraphicalDisk();
	void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
	~GraphicalDisk();
	void setRadius(double radius);
	void setOutlineColor(QColor color);
	void setOutlineWidth(double width);
	void setColor(QColor color);
	void setFillVisible(bool val);

	void setPosition(Vector3D pos);
	void update();

private:
	void createActors();
	void addActors();
	void removeActors();

	vtkRendererPtr mRenderer;

	double mRadius;
	Vector3D mPosition;

	vtkActorPtr mCircleActor;
	vtkSectorSourcePtr mCircleSource;
	vtkActorPtr mOutlineActor;
	vtkSectorSourcePtr mOutlineSource;
	double mOutlineWidth;
	QColor mOutlineColor;
	QColor mColor;
	bool mFillVisible;

};
typedef boost::shared_ptr<GraphicalDisk> GraphicalDiskPtr;



} // namespace cx



#endif // CXGRAPHICALDISK_H
