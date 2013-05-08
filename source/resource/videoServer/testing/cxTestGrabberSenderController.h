#ifndef CXTestGrabberSenderController_H_
#define CXTestGrabberSenderController_H_

#include <QApplication>
#include "cxGrabberSenderDirectLink.h"

/**
 * Helper object for automated control of the CustusX application.
 *
 * \author Ole Vegard Solberg, SINTEF
 * \date Feb 5, 2012
 */
class TestGrabberSenderController : public QObject
{
	Q_OBJECT

public:
	TestGrabberSenderController(QObject* parent);
	void initialize(cx::GrabberSenderDirectLinkPtr grabberBridge);
	bool verify();

private slots:
	void newImageSlot();
	void newUSStatusSlot();

private:
	cx::GrabberSenderDirectLinkPtr mGrabberBridge;
	bool mImageReceived;
	bool mStatusReceived;
};


#endif /* CXTestGrabberSenderController_H_ */
