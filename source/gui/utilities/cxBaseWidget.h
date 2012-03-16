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

class BaseWidget: public QWidget
{
Q_OBJECT

public:
	BaseWidget(QWidget* parent, QString objectName, QString windowTitle) :
		QWidget(parent), mObjectName(objectName), mWindowTitle(windowTitle)
	{
		if (mObjectName=="US Reconstruction")
			std::cout << ":: [" <<  this->objectName().toAscii().constData() << "]" << std::endl;
		this->setObjectName(mObjectName);
		this->setWindowTitle(mWindowTitle);
	}

	virtual ~BaseWidget() {}
	virtual QString defaultWhatsThis() const = 0; ///< Returns a short description of what this widget will do for you.

	QWidget* createMethodWidget(QWidget* inputWidget, QWidget* outputWidget, QString methodname, bool inputChecked =
		false, bool outputVisible = true)
	{
		QWidget* retval = new QWidget(this);
		QVBoxLayout* toplayout = new QVBoxLayout(retval);
		QGridLayout* layout = new QGridLayout();
		toplayout->addLayout(layout);
		toplayout->addStretch();

		QLabel* methodLabel = new QLabel("<b>" + methodname + "</b>");
		QCheckBox* checkBox = new QCheckBox("generate");

		inputWidget->setVisible(inputChecked);
		outputWidget->setVisible(outputVisible);
		connect(checkBox, SIGNAL(clicked(bool)), inputWidget, SLOT(setVisible(bool)));

		layout->addWidget(methodLabel, 0, 0);
		layout->addWidget(checkBox, 0, 1);
		layout->addWidget(inputWidget, 1, 0, 1, 2);
		layout->addWidget(outputWidget, 2, 0, 1, 2);

		return retval;
	}

	QGroupBox* createGroupbox(QWidget* widget, QString boxname)
	{
		QGroupBox* retval = new QGroupBox(this);
		QVBoxLayout* toplayout = new QVBoxLayout(retval);

		QLabel* nameLabel = new QLabel(boxname);
		toplayout->addWidget(nameLabel);
		toplayout->addWidget(widget);

		return retval;
	}

	QFrame* createHorizontalLine() ///< creates a horizontal line witch can be inserted into widgets
	{
		QFrame* retval = new QFrame();
		retval->setFrameStyle(QFrame::Sunken + QFrame::HLine);
		retval->setFixedHeight(12);
		return retval;
	}

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
	void adjustSizeSlot()
	{
		this->parentWidget()->adjustSize();
		this->adjustSize();
	}

protected:
	virtual void showEvent(QShowEvent* event)
	{
		this->setWhatsThis(this->defaultWhatsThis());
		QWidget::showEvent(event);
	}

private:
	QString mObjectName;
	QString mWindowTitle;
};

}
#endif /* CXBASEWIDGET_H_ */
