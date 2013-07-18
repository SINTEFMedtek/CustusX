#ifndef CXOVERLAYWIDGET_H_
#define CXOVERLAYWIDGET_H_

#include "cxBaseWidget.h"

namespace cx {
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/*
 * \class OverlayWidget
 *
 *  \data Jul 18, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class OverlayWidget : public BaseWidget
{
	Q_OBJECT

public:
	OverlayWidget(QWidget* parent);
	virtual QString defaultWhatsThis() const;
};

/**
 * @}
 */
} /* namespace cx */
#endif /* CXOVERLAYWIDGET_H_ */
