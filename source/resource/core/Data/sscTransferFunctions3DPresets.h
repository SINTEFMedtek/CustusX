// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCTRANSFERFUNCTIONS3DPRESET_H_
#define SSCTRANSFERFUNCTIONS3DPRESET_H_

#include <QDomElement>
#include "sscPresets.h"
#include "sscImage.h"
#include "sscXmlOptionItem.h"

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
 * \ingroup sscData
 */
class TransferFunctions3DPresets : public Presets
{
	Q_OBJECT
public:
	TransferFunctions3DPresets(XmlOptionFile presetFile, XmlOptionFile customFile);
	virtual ~TransferFunctions3DPresets(){};

	void save(QString name, ImagePtr image, bool _2D=true, bool _3D=true);
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

#endif /* SSCTRANSFERFUNCTIONS3DPRESET_H_ */
