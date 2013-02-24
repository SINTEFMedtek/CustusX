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
#ifndef CXDATAADAPTERHELPER_H_
#define CXDATAADAPTERHELPER_H_

#include "sscDataAdapter.h"

class QWidget;
class QGridLayout;

namespace cx {

/**\brief Create a widget capable of displaying the input data.
 *
 * If a gridLayout is provided, the widget will insert its components
 * into a row in that layout
 *
 * \ingroup cxGUI
 */
QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout = 0, int row = 0);


} /* namespace cx */
#endif /* CXDATAADAPTERHELPER_H_ */
