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

#ifndef CXVIEW2D_H_
#define CXVIEW2D_H_

#include "sscView.h"

namespace cx
{

/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

/**
 * \class View2D
 *
 * \brief Class for displaying 2D representations.
 *
 * \date Dec 9, 2008
 * \\author Janne Beate Bakeng, SINTEF
 */
class View2D: public ssc::ViewWidget
{
Q_OBJECT
public:
	View2D(const QString& uid, const QString& name = "", QWidget *parent = NULL, Qt::WFlags f = 0); ///< constructor
	virtual ~View2D(); ///< empty
	virtual Type getType() const { return VIEW_2D; } ///< get the class type
};

/**
 * @}
 */
} //namespace cx
#endif /* CXVIEW2D2_H_ */
