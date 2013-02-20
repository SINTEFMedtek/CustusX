/*
 * cxTestGEInterfaceController.h
 *
 *  \date Feb 5, 2012
 *      \author Ole Vegard Solberg
 */
#ifndef CXTESTGEINTERFACECONTROLLER_H_
#define CXTESTGEINTERFACECONTROLLER_H_

#include <QApplication>
#include "cxGrabberSender.h"

/**Helper object for automated control of the CustusX application.
 *
 */
class TestGEInterfaceController : public QObject
{
	Q_OBJECT

public:
	TestGEInterfaceController(QObject* parent);
	void initialize(cx::GrabberSenderDirectLinkPtr grabberBridge);
	void verify();

private slots:
	void newImageSlot();
	void newUSStatusSlot();

private:
	cx::GrabberSenderDirectLinkPtr mGrabberBridge;
	bool mImageReceived;
	bool mStatusReceived;
};


#endif /* CXTESTGEINTERFACECONTROLLER_H_ */
