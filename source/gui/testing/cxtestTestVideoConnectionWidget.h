#ifndef CXTESTTESTVIDEOCONNECTIONWIDGET_H_
#define CXTESTTESTVIDEOCONNECTIONWIDGET_H_

#include "cxVideoConnectionWidget.h"

namespace cxtest
{

/*
 * TestVideoConnectionWidget
 *
 * \date May 27, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class TestVideoConnectionWidget : public cx::VideoConnectionWidget
{
	Q_OBJECT
public:
	TestVideoConnectionWidget();
	virtual ~TestVideoConnectionWidget(){};

public:
	bool canStream(QString filename, QString streamerType);

private:
	void setupWidgetToRunStreamer(QString filename, QString streamerType);
};

} /* namespace cxtest */
#endif /* CXTESTTESTVIDEOCONNECTIONWIDGET_H_ */
