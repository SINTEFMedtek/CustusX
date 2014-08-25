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

#ifndef CXFILTERIMPL_H
#define CXFILTERIMPL_H

#include <vector>
#include <QObject>
#include "cxFilter.h"
#include <QDomElement>

namespace cx
{

/** Helper implementation for Filter.
 *
 * Inherit from this class to get access to useful functionality common
 * to Filter descendants.
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 21, 2012
 * \author Christian Askeland, SINTEF
 */
class FilterImpl : public Filter
{
	Q_OBJECT

public:
	virtual ~FilterImpl() {}

	virtual QString getUid() const;
	virtual void initialize(QDomElement root, QString uid = "");
	virtual std::vector<DataAdapterPtr> getOptions();
	virtual std::vector<SelectDataStringDataAdapterBasePtr> getInputTypes();
	virtual std::vector<SelectDataStringDataAdapterBasePtr> getOutputTypes();
	virtual bool hasPresets(){return false;};
	virtual PresetsPtr getPresets(){ return PresetsPtr();};
	virtual QDomElement generatePresetFromCurrentlySetOptions(QString name){return QDomElement();};
	virtual void setActive(bool on);
	virtual bool preProcess();

public slots:
	virtual void requestSetPresetSlot(QString name){};

protected:
	explicit FilterImpl();

	/** Helper: Return the index'th input type as an image.
	  * Return zero if not available (as image). */
	ImagePtr getCopiedInputImage(int index = 0);
	/** Helper:
      * Call if you have an image threshold dataAdapter that need update from an image change.
	  */
	void updateThresholdFromImageChange(QString uid, DoubleDataAdapterXmlPtr threshold);
	void updateThresholdPairFromImageChange(QString uid, DoublePairDataAdapterXmlPtr threshold);

	virtual void createOptions() = 0;
	virtual void createInputTypes() = 0;
	virtual void createOutputTypes() = 0;

	std::vector<SelectDataStringDataAdapterBasePtr> mInputTypes;
	std::vector<SelectDataStringDataAdapterBasePtr> mOutputTypes;
	std::vector<DataAdapterPtr> mOptionsAdapters;
	QDomElement mOptions;

	// data used by execute - copied for thread safety purposes
	std::vector<DataPtr> mCopiedInput;
	QDomElement mCopiedOptions;
	bool mActive;

private:
	QString mUid;

};

} // namespace cx


#endif // CXFILTERIMPL_H
