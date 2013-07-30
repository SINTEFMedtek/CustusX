#ifndef CXTESTTESTTRANSFERFUNCTIONCOLORWIDGET_H_
#define CXTESTTESTTRANSFERFUNCTIONCOLORWIDGET_H_

#include "cxTransferFunctionColorWidget.h"

namespace cxtest {
/*
 * TestTransferfunctionColorWidget
 *
 *  \date Jul 29, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class TestTransferFunctionColorWidget : public cx::TransferFunctionColorWidget
{
	Q_OBJECT
public:
	TestTransferFunctionColorWidget();

	void initWithTestData();
	void setPlotArea(QRect plotArea);
	int getLeftAreaBoundary();
	int getRigthAreaBoundary();

private:
	int mLeftAreaBoundary;
	int mRightAreaBoundary;
	int mWidthAreaBoundary;
};

} /* namespace cxtest */
#endif /* CXTESTTESTTRANSFERFUNCTIONCOLORWIDGET_H_ */
