#ifndef SSCRECONSTRUCTIONWIDGET_H_
#define SSCRECONSTRUCTIONWIDGET_H_

#include <QtGui>
#include "sscReconstructManager.h"
#include "sscDoubleWidgets.h"
#include "sscXmlOptionItem.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscFileSelectWidget.h"
#include "cxBaseWidget.h"

namespace cx
{
class TimedAlgorithmProgressBar;
}

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginUsReconstruction
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
	ReconstructionWidget(QWidget* parent, ssc::ReconstructManagerPtr reconstructer);
	ssc::ReconstructManagerPtr reconstructer()
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
	void repopulateAlgorithmGroup();
	void reconstructStartedSlot();
	void reconstructFinishedSlot();
	void toggleDetailsSlot();

private:
	ssc::ReconstructManagerPtr mReconstructer;
//	ssc::ThreadedTimedReconstructerPtr mThreadedTimedReconstructer;

	ssc::FileSelectWidget* mFileSelectWidget;
	QPushButton* mReconstructButton;
	QPushButton* mReloadButton;
	QLineEdit* mExtentLineEdit;
	QLineEdit* mInputSpacingLineEdit;
	ssc::SpinBoxGroupWidget* mMaxVolSizeWidget;
	ssc::SpinBoxGroupWidget* mSpacingWidget;
	ssc::SpinBoxGroupWidget* mDimXWidget;
	ssc::SpinBoxGroupWidget* mDimYWidget;
	ssc::SpinBoxGroupWidget* mDimZWidget;
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
#endif //SSCRECONSTRUCTIONWIDGET_H_
