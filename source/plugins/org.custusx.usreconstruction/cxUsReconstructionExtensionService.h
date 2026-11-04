/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXUSRECONSTRUCTIONEXTENSIONSERVICE_H
#define CXUSRECONSTRUCTIONEXTENSIONSERVICE_H

#include "org_custusx_usreconstruction_Export.h"
#include <vector>
#include <QColor>
#include <QObject>
#include <QStringList>
#include "cxForwardDeclarations.h"

class QWidget;

#define UsReconstructionExtensionService_iid "cx::UsReconstructionExtensionService"

namespace cx
{

/** \brief Adds outputs and a settings tab to the US reconstruction widget.
 *
 * The outputs are reconstructed from the recording selected in the UsReconstructionService,
 * after the US volumes.
 *
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT UsReconstructionExtensionService : public QObject
{
	Q_OBJECT
public:
	struct Output
	{
		QString mUid;
		QString mName;
		QString mHelp;
	};

	virtual ~UsReconstructionExtensionService() {}

	virtual QString getName() const = 0; ///< The name of the settings tab
	virtual std::vector<Output> getOutputs() const = 0;
	virtual QWidget* createSettingsWidget(QWidget* parent) = 0;

	virtual bool start(QStringList outputUids) = 0; ///< False if nothing was started. finished() is emitted when a started run ends.
	virtual void cancel() = 0;
	virtual bool isRunning() const = 0;

	virtual QColor getDataColor(DataPtr data) const { Q_UNUSED(data); return QColor(); } ///< The color of data created by the extension, or an invalid color
	virtual void setDataColor(DataPtr data, QColor color) { Q_UNUSED(data); Q_UNUSED(color); }

signals:
	void progress(int done, int total);
	void statusChanged(QString text);
	void finished();
};

} // namespace cx

Q_DECLARE_INTERFACE(cx::UsReconstructionExtensionService, UsReconstructionExtensionService_iid)

#endif // CXUSRECONSTRUCTIONEXTENSIONSERVICE_H
