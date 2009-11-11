#include "sscAcceptanceBoxWidget.h"

#include <iostream>
#include "sscConfig.h"

namespace ssc
{

AcceptanceBoxWidget::AcceptanceBoxWidget(QString text, QWidget* parent) : QFrame(parent)
{
	mAccepted = false;
	QVBoxLayout* top = new QVBoxLayout(this);
	QHBoxLayout* buttons = new QHBoxLayout;
	QLabel* preText = new QLabel("Accept the following statement:");
	preText->setFont(QFont("Arial", 12));
	mText = new QLabel;
	setText(text);
	mText->setFont(QFont("Arial", 14, 75));
	top->addWidget(preText);
	top->addWidget(mText);
	mValue = new QLabel;
	mValue->setFont(QFont("Arial", 14, 75));
	top->addWidget(mValue);

	top->addLayout(buttons);
	mAcceptButton = new QPushButton("Accept");
	mRejectButton = new QPushButton("Reject");
	buttons->addWidget(mAcceptButton);
	//buttons->addStretch();
	buttons->addWidget(mRejectButton);
	connect(mAcceptButton, SIGNAL( clicked()), this, SLOT(accept()) );
	connect(mRejectButton, SIGNAL( clicked()), this, SLOT(reject()) );
	mAcceptButton->setFocus();
	//mAcceptButton->setShortcut(Qt::Key_Enter);
}

void AcceptanceBoxWidget::hideAcceptButtons()
{
	mAcceptButton->setVisible(false);
	mRejectButton->setVisible(false);
}
void AcceptanceBoxWidget::setValue(double val)
{
	mText->setNum( val );
}

void AcceptanceBoxWidget::setText(QString text)
{
	//text = "Accept the following statement:\n" + text;

#ifdef SSC_AUTOMATIC_TEST_ACCEPT
	text += "\n[Auto mode: Accepted after " + QString::number(SSC_DEFAULT_TEST_TIMEOUT_SECS) + "s]";
#endif

	mText->setText(text);
}

bool AcceptanceBoxWidget::accepted() const
{
	return mAccepted;
}

/**trigger an event when shown. This gives visibility of the
 * widget in a specified time (putting it in the constructor will
 * include first render time in the shown time)
 */
void AcceptanceBoxWidget::showEvent ( QShowEvent * event )
{
	QWidget::showEvent(event);

	// autofinish if auto (this define lies in ssc/Code/Utilities/sscConfig.h.in)
	#ifdef SSC_AUTOMATIC_TEST_ACCEPT
	std::cout << "autofinishing..." << std::endl;
	QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, this, SLOT(accept())); // terminate app after some seconds - this is an automated test!!
	#endif
}

void AcceptanceBoxWidget::accept()
{
	finish(true);
}

void AcceptanceBoxWidget::reject()
{
	finish(false);
}

void AcceptanceBoxWidget::finish(bool accepted)
{
	mAccepted = accepted;
	std::cout << "success: " << accepted << std::endl;
	emit finished(mAccepted);
	qApp->quit();
}



} // namespace ssc
