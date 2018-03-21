/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

/** \brief Handles transfer function presets
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
