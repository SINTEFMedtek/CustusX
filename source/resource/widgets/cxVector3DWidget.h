/*
 * cxVector3DWidget.h
 *
 *  \date Jul 25, 2011
 *      \author christiana
 */

#ifndef CXVECTOR3DWIDGET_H_
#define CXVECTOR3DWIDGET_H_

#include "cxVector3D.h"
#include <QWidget>
class QBoxLayout;
#include "cxVector3DDataAdapter.h"
#include "cxDoubleDataAdapterXml.h"

namespace cx
{


/**
 * \brief Widget for displaying a Vector3D
 *
 * \ingroup cx_resource_widgets
 *
 */
class Vector3DWidget: public QWidget
{
Q_OBJECT
public:
	Vector3DWidget(QWidget* parent, Vector3DDataAdapterPtr data);
	virtual ~Vector3DWidget() {}

	static Vector3DWidget* createSmallHorizontal(QWidget* parent, Vector3DDataAdapterPtr data);
	static Vector3DWidget* createVerticalWithSliders(QWidget* parent, Vector3DDataAdapterPtr data);
	void showDim(int dim, bool visible);

private:
	void addSliderControlsForIndex(QString name, QString help, int index, QBoxLayout* layout);
	void addSmallControlsForIndex(QString name, QString help, int index, QBoxLayout* layout);

	Vector3DDataAdapterPtr mData;
	boost::array<DoubleDataAdapterPtr, 3> mDoubleAdapter;
	boost::array<QWidget*, 3> mWidgets;
};

}

#endif /* CXVECTOR3DWIDGET_H_ */
