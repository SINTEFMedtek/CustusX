// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCHELPERWIDGETS_H_
#define SSCHELPERWIDGETS_H_

#include <QWidget>
#include "sscDataAdapter.h"
class QGridLayout;

namespace ssc
{

/**\brief A widget that floats above the other windows. Used as basis for the floating controls.
 *
 * \ingroup sscWidget
 */
class sscFloatingWidget: public QWidget
{
Q_OBJECT

public:
	virtual ~sscFloatingWidget()
	{
	}
	sscFloatingWidget(QWidget* parent = 0) :
		QWidget(parent)
	{
		Qt::WindowFlags flags = Qt::FramelessWindowHint;
		flags |= Qt::Window;
		flags |= Qt::WindowStaysOnTopHint;
		this->setWindowFlags(flags);
	}
};

/**\brief Create a widget capable of displaying the input data.
 *
 * If a gridLayout is provided, the widget will insert its components
 * into a row in that layout
 *
 * \ingroup sscWidget
 */
QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout = 0, int row = 0);
}

#endif
