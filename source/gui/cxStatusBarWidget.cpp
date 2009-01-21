#include "cxStatusBarWidget.h"

#include <QHBoxLayout>
/*
 * cxStatusBarWidget.cpp
 *
 *  Created on: Jan 21, 2009
 *      Author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
StatusBarWidget::StatusBarWidget() :
	mHBoxLayout(new QHBoxLayout())
{
	this->setLayout(mHBoxLayout);
}
StatusBarWidget::~StatusBarWidget()
{}
}
