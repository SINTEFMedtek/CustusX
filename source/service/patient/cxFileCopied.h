// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXFILECOPIED_H_
#define CXFILECOPIED_H_

#include <string>
#include <QObject>
#include "sscForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

/**
 * \brief This is a helper class for MainWindow::importDataSlot().
 * Contain a slot that checks if a file is finished copying and updates the
 * ssc:Image object
 * \ingroup cxServicePatient
 *
 * \date Feb 8, 2010
 * \author Ole Vegard Solberg, SINTEF
 */
class FileCopied: public QObject
{
Q_OBJECT
public:
	FileCopied(const QString& absolutefilePath, const QString& relativefilePath, ssc::DataPtr data); ///< Creates the object

signals:
	void fileCopiedCorrectly(); ///< Sends a signal when the copy is verified
protected:
	QString mFilePath;
	QString mRelativeFilePath;
	ssc::DataPtr mData;
public slots:
	void areFileCopiedSlot(); ///< Checks if the file is finished copying and updates the ssc::Image
};

/**
 * @}
 */
} // namespace cx

#endif /* CXFILECOPIED_H_ */
