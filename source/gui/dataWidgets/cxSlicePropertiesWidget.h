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
#ifndef CXSlicePropertiesWidget_H_
#define CXSlicePropertiesWidget_H_

#include <vector>
#include <QtGui>
#include "cxTabbedWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * \class SlicePropertiesWidget
 *
 * \date 2010.04.12
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class SlicePropertiesWidget : public TabbedWidget
{
  Q_OBJECT

public:
	SlicePropertiesWidget(QWidget* parent);
	virtual ~SlicePropertiesWidget();
  virtual QString defaultWhatsThis() const;

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
	SlicePropertiesWidget();
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXSlicePropertiesWidget_H_ */
