/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREADFBGSMESSAGE_H
#define CXREADFBGSMESSAGE_H

#include <vector>
#include <QString>
#include <QColor>
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"

namespace cx
{
/**
 * Class for decoding the messages from the FBGS, and creating a vtkPolyData.
 *
 * \ingroup org_custusx_tracking_shape
 *
 * \date 2022-11-09
 * \author Ole Vegard Solberg
 */
class ReadFbgsMessage
{
public:
	enum AXIS
	{
		axisX,
		axisY,
		axisZ,
		axisCOUNT
	};
	ReadFbgsMessage();
	void readBuffer(QString buffer);
	void setColor(QColor color);
	vtkPolyDataPtr getPolyData();
	vtkActorPtr getActor();
	void set_rMpr(Transform3D rMpr);
protected:
	vtkPolyDataPtr mPolyData; ///< polydata representation of the probe, in space u
	vtkActorPtr mActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
	vtkPropertyPtr mProperty;
	vtkPointsPtr mPoints;
	vtkCellArrayPtr mLines;

	std::vector<AXIS> mAxis;
	std::vector<double> mXaxis;
	std::vector<double> mYaxis;
	std::vector<double> mZaxis;

	bool readShape(AXIS axis, QString buffer);
	bool readPositions(AXIS axis, QString buffer, int bufferPos);
	QString getAxisString(AXIS axis);
	std::vector<double> getAxisPosVector(AXIS axis);
	void createPolyData();
	void clearPolyData();
	void clearAxisVectors();
};

}//cx
#endif // CXREADFBGSMESSAGE_H
