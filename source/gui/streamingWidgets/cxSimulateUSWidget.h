#ifndef CXSIMULATEUSWIDGET_H_
#define CXSIMULATEUSWIDGET_H_

#include "cxBaseWidget.h"

#include "boost/shared_ptr.hpp"

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
	void imageChangedSlot(QString imageUid);

private:
	QVBoxLayout* mTopLayout;
	SelectImageStringDataAdapterPtr mImageSelector;

};

} /* namespace cx */
#endif /* CXSIMULATEUSWIDGET_H_ */
