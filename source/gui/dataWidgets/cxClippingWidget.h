#ifndef CXCLIPPINGWIDGET_H_
#define CXCLIPPINGWIDGET_H_

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxStringDataAdapter.h"
//#include "cxViewManager.h"
//#include "cxDataInterface.h"
class QCheckBox;

namespace cx
{
typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;

typedef boost::shared_ptr<class InteractiveClipper> InteractiveClipperPtr;

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Adapter that connects to the current active image.
 */
class ClipPlaneStringDataAdapter: public StringDataAdapter
{
Q_OBJECT
public:
	static StringDataAdapterPtr New(InteractiveClipperPtr clipper)
	{
		return StringDataAdapterPtr(new ClipPlaneStringDataAdapter(clipper));
	}
	ClipPlaneStringDataAdapter(InteractiveClipperPtr clipper);
	virtual ~ClipPlaneStringDataAdapter() {}

public:
	// basic methods
	virtual QString getValueName() const;
	virtual bool setValue(const QString& value);
	virtual QString getValue() const;

public:
	// optional methods
	virtual QString getHelp() const;
	virtual QStringList getValueRange() const;

	InteractiveClipperPtr mInteractiveClipper;
};

/*
 * \class ClippingWidget
 *
 * \date Aug 25, 2010
 * \author Christian Askeland, SINTEF
 */

class ClippingWidget: public BaseWidget
{
Q_OBJECT

public:
	ClippingWidget(QWidget* parent);
	virtual QString defaultWhatsThis() const;

private:
	InteractiveClipperPtr mInteractiveClipper;

	QCheckBox* mUseClipperCheckBox;
	QCheckBox* mInvertPlaneCheckBox;
	StringDataAdapterPtr mPlaneAdapter;
	SelectImageStringDataAdapterPtr mImageAdapter;
private slots:
	void clipperChangedSlot();
	void clearButtonClickedSlot();
	void saveButtonClickedSlot();
	void imageChangedSlot();
};

/**
 * @}
 */
}//namespace cx

#endif /* CXCLIPPINGWIDGET_H_ */
