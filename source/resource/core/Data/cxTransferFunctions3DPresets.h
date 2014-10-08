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


#ifndef CXTRANSFERFUNCTIONS3DPRESETS_H_
#define CXTRANSFERFUNCTIONS3DPRESETS_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <QDomElement>
#include "cxPresets.h"
#include "cxImage.h"
#include "cxXmlOptionItem.h"

class QString;
class QStringList;
class QDomDocument;

namespace cx
{

typedef boost::shared_ptr<class TransferFunctions3DPresets> TransferFunctions3DPresetsPtr;

/**
 * \date 11. juni 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

/**\brief Handles transfer function presets
 *
 * \ingroup cx_resource_core_data
 */
class cxResource_EXPORT TransferFunctions3DPresets : public Presets
{
	Q_OBJECT
public:
	TransferFunctions3DPresets(XmlOptionFile presetFile, XmlOptionFile customFile);
	virtual ~TransferFunctions3DPresets(){};

	virtual void save(QString name, ImagePtr image, bool _2D=true, bool _3D=true);
	void load(QString name, ImagePtr image, bool _2D=true, bool _3D=true);

	void save2D(QString name, ImagePtr image);
	void save3D(QString name, ImagePtr image);
	void load2D(QString name, ImagePtr image);
	void load3D(QString name, ImagePtr image);

	void deletePresetData(QString name, bool _2D=true, bool _3D=true); ///< Delete the preset data node
protected:
	QStringList generatePresetList(QString modality); ///< internally generate the preset list

private:
};

}//namespace cx

#endif /* CXTRANSFERFUNCTIONS3DPRESETS_H_ */
