/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCONTROLLABLESPLITTER_H
#define CXCONTROLLABLESPLITTER_H

#include "cxXmlOptionItem.h"
#include <QWidget>
class QSplitter;

namespace cx
{

/** A splitter with two subwidgets, with actions for swapping
 * between them.
 *
 */
class ControllableSplitter : public QWidget
{
	Q_OBJECT
public:
	ControllableSplitter(XmlOptionFile options, QWidget* parent);
	~ControllableSplitter();
	void addLeftWidget(QWidget* widget, QString name);
	void addRightWidget(QWidget* widget, QString name);
	QAction* getMoveLeftAction();
	QAction* getMoveRightAction();

private slots:
	void initializeSettings();

private:
	void onMoveSplitterLeft();
	void onMoveSplitterRight();
	void shiftSplitter(int shift);
	bool splitterShowsBoth() const;
	void onSplitterMoved();

	int getShiftState() const;
	void setShiftState(int shiftState);
	void enableActions();

	XmlOptionItem getSplitterRatioOption();
	XmlOptionItem getShiftStateOption();

	QSplitter* mSplitter;
	QAction* mShiftSplitterLeft;
	QAction* mShiftSplitterRight;
	XmlOptionFile mOptions;
	QString mLeftName;
	QString mRightName;

	double mSplitterRatio;
};

}//end namespace cx

#endif // CXCONTROLLABLESPLITTER_H
