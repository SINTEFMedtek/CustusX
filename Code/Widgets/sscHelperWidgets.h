#ifndef SSCHELPERWIDGETS_H_
#define SSCHELPERWIDGETS_H_

#include <QWidget>

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



}

#endif
