#ifndef CXBASEWIDGET_H_
#define CXBASEWIDGET_H_

#include <QWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <iostream>
#include "sscOptimizedUpdateWidget.h"

namespace cx
{
/**
 * \class BaseWidget
 *
 * \brief Interface for QWidget which handles widgets uniformly for the system.
 * \ingroup cxGUI
 *
 * \date 28. sep. 2010
 * \\author Janne Beate Bakeng
 */

class BaseWidget: public ssc::OptimizedUpdateWidget
{
Q_OBJECT

public:
    BaseWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~BaseWidget() {}
	virtual QString defaultWhatsThis() const = 0; ///< Returns a short description of what this widget will do for you.

    /**
      * Create a specialized widget for filters, with input/ouput, enable and options.
      */
    QWidget* createMethodWidget(QWidget* inputWidget, QWidget* outputWidget, QString methodname, bool inputChecked =
        false, bool outputVisible = true);
    /**
      * Create a group box with a given name.
      */
    QGroupBox* createGroupbox(QWidget* widget, QString boxname);
    /**
      * Creates a horizontal line which can be inserted into widgets
      */
    QFrame* createHorizontalLine();

	/**Shorthand method for action creation.
	 * If layout is used, a QToolButton is created and added to it.
	 * If tip is empty, it is set equal to text.
	 */
	template<class T>
	QAction* createAction(QObject* parent, QIcon iconName, QString text, QString tip, T slot, QLayout* layout=NULL)
	{
		if (tip.isEmpty())
			tip = text;
	  QAction* action = new QAction(iconName, text, parent);
	  action->setStatusTip(tip);
	  action->setWhatsThis(tip);
	  action->setToolTip(tip);
	  connect(action, SIGNAL(triggered()), this, slot);
	  if (layout)
	  {
		  QToolButton* button = new QToolButton();
		  button->setDefaultAction(action);
		  layout->addWidget(button);
	  }
	  return action;
	}


public slots:
    void adjustSizeSlot();

protected:
    virtual void showEvent(QShowEvent* event);

private:
    QString mObjectName;
	QString mWindowTitle;
};

}
#endif /* CXBASEWIDGET_H_ */
