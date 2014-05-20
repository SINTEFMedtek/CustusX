// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXSIMULATEUSWIDGET_H_
#define CXSIMULATEUSWIDGET_H_

#include "cxBaseWidget.h"

#include "boost/shared_ptr.hpp"

#include "cxXmlOptionItem.h"
#include "cxStringDataAdapterXml.h"
#include "cxSimulatedImageStreamerInterface.h"

class QVBoxLayout;

namespace cx
{
typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;

/**
 * \brief Gui for interacting with us simulation.
 *
 * \date Jun 18, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class SimulateUSWidget : public BaseWidget
{
	Q_OBJECT

public:
	SimulateUSWidget(QWidget* parent = NULL);
	~SimulateUSWidget();

	virtual QString defaultWhatsThis() const;

	QString getImageUidToSimulate() const;
	void setImageUidToSimulate(QString uid);

signals:
	void imageSelected();

private slots:
	void simulationTypeChanged();
	void imageChangedSlot(QString imageUid);
	void gainChanged(int gain);

private:
	void createAndAddGainController();
	SimulatedImageStreamerInterfacePtr getStreamerInterface();

	QVBoxLayout* mTopLayout;
	SelectImageStringDataAdapterPtr mImageSelector;

	StringDataAdapterXmlPtr mSimulationType;
	SimulatedImageStreamerInterfacePtr mSimulatedStreamerInterface;

};

} /* namespace cx */
#endif /* CXSIMULATEUSWIDGET_H_ */
