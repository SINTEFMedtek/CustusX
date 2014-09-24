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

#ifndef CXVECTOR3DCOMPONENTDATAADAPTER_H_
#define CXVECTOR3DCOMPONENTDATAADAPTER_H_

#include "cxResourceExport.h"

#include "cxDoubleDataAdapter.h"
#include "cxVector3DDataAdapter.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_dataadapters
 * @{
 */

/**A data adapter that links to one component of a Vector3DDataAdapter.
 * Useful for displaying widgets for vector components.
 *
 */
class cxResource_EXPORT Vector3DComponentDataAdapter : public DoubleDataAdapter
{
  Q_OBJECT
public:
  Vector3DComponentDataAdapter(Vector3DDataAdapterPtr base, int index, QString name, QString help);
  virtual ~Vector3DComponentDataAdapter() {}

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

  Vector3DDataAdapterPtr mBase;
  int mIndex;
  QString mName;
  QString mHelp;
};
typedef boost::shared_ptr<Vector3DComponentDataAdapter> Vector3DComponentDataAdapterPtr;


/**
 * @}
 */
}

#endif /* CXVECTOR3DCOMPONENTDATAADAPTER_H_ */
