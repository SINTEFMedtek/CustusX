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

#ifndef CXVIEW3D2_H_
#define CXVIEW3D2_H_

#include "sscView.h"
//#include "sscTransform3D.h"
//#include "sscTool.h"
#include "sscForwardDeclarations.h"

class QWidget;
class QMenu;

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

/**
 * \class View3D
 *
 * \brief Class for displaying 3D reps.
 *
 * \date Dec 9, 2008
 * \\author Janne Beate Bakeng, SINTEF
 */
class View3D: public ssc::View
{
Q_OBJECT
public:
	View3D(const QString& uid, const QString& name = "", QWidget *parent = NULL, Qt::WFlags f = 0); ///< constructor
	virtual ~View3D(); ///< empty
	virtual Type getType() const { return VIEW_3D; }  ///< get the class type

public slots:
protected slots:
protected:
};

/**
 * @}
 */
} //namespace cx
#endif /* CXVIEW3D2_H_ */

