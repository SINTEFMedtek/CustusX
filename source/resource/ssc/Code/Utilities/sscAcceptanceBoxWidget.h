#ifndef SSCACCEPTANCEBOXWIDGET_H_
#define SSCACCEPTANCEBOXWIDGET_H_

#include <QtGui>

namespace ssc
{

/**\brief Widget for semi-automatic testing of visuals.
 *
 * Widget that displays two buttons: accept and reject.
 * The result will emit a finished(bool) signal. - true=accepted, false=rejected
 *
 * Intended use: During semiautomated tests, the user can accept/reject the graphical
 * test result using these buttons. Connect the signal to the return value of the
 * test function or similar.
 *
 * Clicking the button will also send a quit() to the application.
 *
 * In automatic mode, the accept button is clicked automatically after 2 seconds.
 *
 * Automatic/semiautomatic mode is switched in
 *     ssc/Code/Utilities/sscConfig.h.in
 *
 * \ingroup sscUtility
 */
class AcceptanceBoxWidget : public QFrame
{
	Q_OBJECT

public:
	AcceptanceBoxWidget(QString text, QWidget* parent = 0);
	virtual ~AcceptanceBoxWidget() {}
	bool accepted() const;
	void setText(QString text);
	void setValue(double val);
	void hideAcceptButtons();

signals:
	void finished(bool);

private:
	bool mAccepted;
	void finish(bool accepted);
	QLabel* mText;
	QLabel* mValue;
	QPushButton* mAcceptButton;
	QPushButton* mRejectButton;
	bool mAutoTest;

protected:
	virtual void showEvent(QShowEvent * event);

private slots:
	void accept();
	void reject();
};



}

#endif /*SSCACCEPTANCEBOXWIDGET_H_*/
