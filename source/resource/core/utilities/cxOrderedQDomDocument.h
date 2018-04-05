/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef ORDEREDQDOMDOCUMENT_H
#define ORDEREDQDOMDOCUMENT_H

#include <QDomDocument>
#include <QHash>


/** A QDomDocument that stores attributes in the same order each time.
 *
 * Set a fixed hash seed in order to fix the ordering of xml attributes
 * http://stackoverflow.com/questions/21535707/qtxml-incorrect-order-of-attributes
 * needed primarily because we store the session in git and would like to diff.
 */
class OrderedQDomDocument
{
public:
	OrderedQDomDocument()
	{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
		// set fixed hash seed
		qSetGlobalQHashSeed(42);
#endif
		mDoc = QDomDocument();
	}
	~OrderedQDomDocument()
	{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
		// reset hash seed with new random value.
		qSetGlobalQHashSeed(-1);
#endif
	}

	QDomDocument& doc() {return mDoc; }
private:
	QDomDocument mDoc;
};

#endif // ORDEREDQDOMDOCUMENT_H
