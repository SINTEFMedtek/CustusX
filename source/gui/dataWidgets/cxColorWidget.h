#ifndef CXCOLORWIDGET_H_
#define CXCOLORWIDGET_H_

#include "cxBaseWidget.h"

namespace cx {
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/*
 * \class ColorWidget
 *
 *  \data Jul 18, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class ColorWidget : public BaseWidget
{
	Q_OBJECT

public:
	ColorWidget(QWidget* parent);
	virtual QString defaultWhatsThis() const;
};

/**
 * @}
 */
} /* namespace cx */
#endif /* CXCOLORWIDGET_H_ */
