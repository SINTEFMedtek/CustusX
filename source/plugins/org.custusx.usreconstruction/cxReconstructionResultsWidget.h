/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRECONSTRUCTIONRESULTSWIDGET_H
#define CXRECONSTRUCTIONRESULTSWIDGET_H

#include "org_custusx_usreconstruction_Export.h"
#include <functional>
#include <vector>
#include <QColor>
#include <QWidget>
#include "cxForwardDeclarations.h"

class QAction;
class QVBoxLayout;

namespace cx
{

/** \brief The data created by a reconstruction, each with its type icon, a button to show it in the active
 * view group, a button to delete it, and a color button for meshes and for data with a color from the color functions.
 *
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT ReconstructionResultsWidget : public QWidget
{
	Q_OBJECT
public:
	typedef std::function<QColor(DataPtr)> ColorGetter; ///< An invalid color if the data has no color
	typedef std::function<void(DataPtr, QColor)> ColorSetter;

	ReconstructionResultsWidget(PatientModelServicePtr patientModel, ViewServicePtr view, QWidget* parent = NULL);

	void setColorFunctions(ColorGetter getter, ColorSetter setter);

	void setData(std::vector<DataPtr> data);
	QStringList getUids() const;

private slots:
	void dataAddedOrRemoved();
	void activeViewChanged();
	void updateShowButtons();

private:
	struct Row
	{
		QString mUid;
		QAction* mShow;
		QWidget* mWidget;
	};
	Row createRow(DataPtr data);
	void toggleShow(QString uid, bool show);
	ViewGroupDataPtr getViewGroup() const;
	QColor getColor(DataPtr data) const;
	void setColor(DataPtr data, QColor color);

	PatientModelServicePtr mPatientModel;
	ViewServicePtr mView;
	ViewGroupDataPtr mViewGroup;
	QVBoxLayout* mLayout;
	std::vector<Row> mRows;
	ColorGetter mColorGetter;
	ColorSetter mColorSetter;
};

} // namespace cx

#endif // CXRECONSTRUCTIONRESULTSWIDGET_H
