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
#ifndef CXLAYOUTREPOSITORY_H
#define CXLAYOUTREPOSITORY_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include "cxForwardDeclarations.h"
#include <vector>
#include "cxXmlOptionItem.h"
#include "cxLayoutData.h"

namespace cx
{
class LayoutData;

/** Repository for View Layouts.
 *
 * Each layout is a separate configuration of 2D/3D/Video views, laid out
 * in a specific way on screen.
 *
 * \ingroup cx_resource_view
 * \date 2014-02-07
 * \author christiana
 */
class cxResourceVisualization_EXPORT LayoutRepository : public QObject
{
	Q_OBJECT
public:
	LayoutRepository();

	LayoutData get(const QString uid) const;
	std::vector<QString> getAvailable() const;
	void insert(const LayoutData& data);
	QString generateUid() const;
	void erase(const QString uid);
	bool isCustom(const QString& uid) const;

	void load(XmlOptionFile file);
	void save(XmlOptionFile file);

	void addDefault(LayoutData data);
	bool exists(const QString uid) const;
signals:
	void layoutChanged(QString uid);
private:
	void addDefaults();
	unsigned indexOf(const QString uid) const;

	typedef std::vector<LayoutData> LayoutDataVector;
	LayoutDataVector mLayouts;
	std::vector<QString> mDefaultLayouts;
};

} // namespace cx


#endif // CXLAYOUTREPOSITORY_H
