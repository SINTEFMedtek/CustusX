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
