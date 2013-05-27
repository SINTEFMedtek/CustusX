#ifndef CXTESTTESTVIDEOCONNECTIONWIDGET_H_
#define CXTESTTESTVIDEOCONNECTIONWIDGET_H_

#include "cxVideoConnectionWidget.h"

namespace cxtest
{

/*
 * TestVideoConnection
 *
 * \date May 27, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class TestVideoConnection : public cx::VideoConnectionWidget
{
	Q_OBJECT

public:
	TestVideoConnection();
	virtual ~TestVideoConnection(){};

public slots:
	bool startStopServer();
};

} /* namespace cxtest */
#endif /* CXTESTTESTVIDEOCONNECTIONWIDGET_H_ */
