/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXSPACEEDITWIDGET_H
#define CXSPACEEDITWIDGET_H

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "cxStringDataAdapter.h"
#include "cxBaseWidget.h"

namespace cx
{

/**\brief Composite widget for string selection.
 *
 * Consists of <namelabel, combobox>.
 * Insert a subclass of StringDataAdStringDataAdapter to connect to data.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpaceEditWidget: public BaseWidget
{
Q_OBJECT
public:
	SpaceEditWidget(QWidget* parent, StringDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
	virtual ~SpaceEditWidget() {}

	virtual QString defaultWhatsThis() const;

	void showLabel(bool on);

protected:
	QHBoxLayout* mTopLayout;

private slots:
	void prePaintEvent();
	void comboIndexChanged();

private:
	struct SpaceData
	{
		SpaceData(QString uid_, QString name_) : uid(uid_), name(name_) {}
		QString uid; // internal value for space
		QString name; // display value for space

		QStringList uidlist() const { return this->splitSpace(uid); }
		QStringList namelist() const { return this->splitSpace(name); }

		QStringList splitSpace(QString name) const
		{
			QStringList values = name.split("/");
			if (values.size()==1)
				values << "";
			return values;
		}
	};

	void rebuildCombobox(QComboBox* widget, QStringList uids, int index, std::vector<SpaceData> allSpaces, QString currentUid);
	QString getNameForUid(QString uid, int index, std::vector<SpaceData> allSpaces);

	QStringList splitSpace(QString name) const;
	QString mergeSpace(QString a, QString b) const;

	QLabel* mLabel;
	QComboBox* mIdCombo;
	QComboBox* mRefCombo;
	StringDataAdapterPtr mData;
};

} // namespace cx

#endif // CXSPACEEDITWIDGET_H
