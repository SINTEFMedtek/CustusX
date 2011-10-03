#ifndef SSCRECONSTRUCTIONWIDGET_H_
#define SSCRECONSTRUCTIONWIDGET_H_

#include <QtGui>
#include "sscReconstructer.h"
#include "sscDoubleWidgets.h"
#include "sscXmlOptionItem.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscFileSelectWidget.h"

namespace ssc
{

/**
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */
class ReconstructionWidget: public QWidget
{
Q_OBJECT
public:
	ReconstructionWidget(QWidget* parent, ReconstructerPtr reconstructer);
	ReconstructerPtr reconstructer()
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

private:
	ReconstructerPtr mReconstructer;

	FileSelectWidget* mFileSelectWidget;
	QPushButton* mReconstructButton;
	QPushButton* mReloadButton;
	QLineEdit* mExtentLineEdit;
	QLineEdit* mInputSpacingLineEdit;
	ssc::SpinBoxGroupWidget* mMaxVolSizeWidget;
	ssc::SpinBoxGroupWidget* mSpacingWidget;
	ssc::SpinBoxGroupWidget* mDimXWidget;
	ssc::SpinBoxGroupWidget* mDimYWidget;
	ssc::SpinBoxGroupWidget* mDimZWidget;

	QGroupBox* mAlgorithmGroup;
	QStackedLayout* mAlgoLayout;
	std::vector<QWidget*> mAlgoWidgets;

	QString getCurrentPath();
	void updateComboBox();
};

}//namespace
#endif //SSCRECONSTRUCTIONWIDGET_H_
