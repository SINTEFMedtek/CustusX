/*
 * cxSecondaryMainWindow.h
 *
 *  \date Oct 18, 2010
 *      \author christiana
 */
#ifndef CXSECONDARYMAINWINDOW_H_
#define CXSECONDARYMAINWINDOW_H_

#include <QMainWindow>

namespace cx
{

/**
 * \brief Experimental class for IPad usage.
 * \ingroup cx_gui
 *
 * This detached main window can be moved onto a secondary screen.
 * The use case was a IPad where scrolling etc could control the
 * CustusX scene.
 *
 */
class SecondaryMainWindow: public QMainWindow
{
Q_OBJECT

public:
	SecondaryMainWindow(QWidget* parent);
	~SecondaryMainWindow();
private:
	void addAsDockWidget(QWidget* widget, bool visible);
};

} // namespace cx

#endif /* CXSECONDARYMAINWINDOW_H_ */
