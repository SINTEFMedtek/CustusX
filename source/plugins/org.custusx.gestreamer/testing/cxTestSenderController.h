#ifndef CXTestGrabberSenderController_H_
#define CXTestGrabberSenderController_H_

#include <QApplication>
#include "cxDirectlyLinkedSender.h"

/**
 * Helper object for automated control of the CustusX application.
 *
 * \author Ole Vegard Solberg, SINTEF
 * \date Feb 5, 2012
 */
class TestSenderController : public QObject
{
	Q_OBJECT

public:
	TestSenderController(QObject* parent);
	void initialize(cx::DirectlyLinkedSenderPtr grabberBridge);
	bool verify();

private slots:
	void newImageSlot();
	void newUSStatusSlot();

private:
	cx::DirectlyLinkedSenderPtr mGrabberBridge;
	bool mImageReceived;
	bool mStatusReceived;
};


#endif /* CXTestGrabberSenderController_H_ */
