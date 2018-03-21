/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVECTOR3DCOMPONENTPROPERTY_H_
#define CXVECTOR3DCOMPONENTPROPERTY_H_

#include "cxResourceExport.h"

#include "cxDoublePropertyBase.h"
#include "cxVector3DPropertyBase.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_properties
 * @{
 */

/**A property that links to one component of a Vector3DProperty.
 * Useful for displaying widgets for vector components.
 *
 */
class cxResource_EXPORT Vector3DComponentProperty : public DoublePropertyBase
{
  Q_OBJECT
public:
  Vector3DComponentProperty(Vector3DPropertyBasePtr base, int index, QString name, QString help);
  virtual ~Vector3DComponentProperty() {}

public: // basic methods
	virtual QString getDisplayName() const;
	virtual bool setValue(double value);
	virtual double getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual DoubleRange getValueRange() const;
  virtual double convertInternal2Display(double internal);
  virtual double convertDisplay2Internal(double display);
  virtual int getValueDecimals() const;

  Vector3DPropertyBasePtr mBase;
  int mIndex;
  QString mName;
  QString mHelp;
};
typedef boost::shared_ptr<Vector3DComponentProperty> Vector3DComponentPropertyPtr;


/**
 * @}
 */
}

#endif /* CXVECTOR3DCOMPONENTPROPERTY_H_ */
