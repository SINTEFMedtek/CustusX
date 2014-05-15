#ifndef CXRECONSTRUCTIONWIDGET_H_
#define CXRECONSTRUCTIONWIDGET_H_

#include <QtGui>
#include "cxReconstructionManager.h"
#include "cxDoubleWidgets.h"
#include "cxXmlOptionItem.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxFileSelectWidget.h"
#include "cxBaseWidget.h"

namespace cx
{
class TimedAlgorithmProgressBar;
}

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_usreconstruction
 * @{
 */

/**
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */
class ReconstructionWidget: public BaseWidget
{
Q_OBJECT
public:
	ReconstructionWidget(QWidget* parent, ReconstructionManagerPtr reconstructer);
	ReconstructionManagerPtr reconstructer()
	{
		return mReconstructer;
	}

public slots:
	void selectData(QString inputfile);
	void reconstruct();
	void reload();
	void paramsChangedSlot();
private slots:
	void inputDataSelected(QString mhdFileName);
	/** Add the widgets for the current algorithm to a stacked widget.*/
	void repopulateAlgorithmGroup();
	void reconstructStartedSlot();
	void reconstructFinishedSlot();
	void toggleDetailsSlot();

	void reconstructAboutToStartSlot();

private:
	ReconstructionManagerPtr mReconstructer;

	FileSelectWidget* mFileSelectWidget;
	QPushButton* mReconstructButton;
	QPushButton* mReloadButton;
	QLineEdit* mExtentLineEdit;
	QLineEdit* mInputSpacingLineEdit;
	SpinBoxGroupWidget* mMaxVolSizeWidget;
	SpinBoxGroupWidget* mSpacingWidget;
	SpinBoxGroupWidget* mDimXWidget;
	SpinBoxGroupWidget* mDimYWidget;
	SpinBoxGroupWidget* mDimZWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

	QFrame* mAlgorithmGroup;
	QStackedLayout* mAlgoLayout;
	std::vector<QWidget*> mAlgoWidgets;

	QWidget* mOptionsWidget;
	QWidget* createOptionsWidget();
	QString getCurrentPath();
	void updateComboBox();
	QString defaultWhatsThis() const;
};

/**
 * @}
 */
}//namespace
#endif //CXRECONSTRUCTIONWIDGET_H_
