#ifndef SSCHELPERWIDGETS_H_
#define SSCHELPERWIDGETS_H_

#include <QWidget>
#include "sscDataAdapter.h"
class QGridLayout;

namespace ssc
{

/** A widget that floats above the other windows. Used as basis for the floating controls. */
class sscFloatingWidget : public QWidget
{
	Q_OBJECT

public:
	virtual ~sscFloatingWidget() {}
	sscFloatingWidget(QWidget* parent = 0) : QWidget(parent)
	{
		Qt::WindowFlags flags = Qt::FramelessWindowHint;
		flags |= Qt::Window;
		flags |= Qt::WindowStaysOnTopHint;
		this->setWindowFlags(flags);
	}
};

/**Create a widget capable of displaying the input data.
 * If a gridLayout is provided, the widget will insert its components
 * into a row in that layout
 *
 */
QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout=0, int row=0);
}

#endif
