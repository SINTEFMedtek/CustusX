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

#ifndef CXFILTERPRESETWIDGET_H_
#define CXFILTERPRESETWIDGET_H_

#include "cxPluginAlgorithmExport.h"

#include "cxPresetWidget.h"

#include "cxForwardDeclarations.h"

namespace cx {

/**
 * \class FilterPresetWidget
 *
 * \brief
 *
 * \ingroup cx_module_algorithm
 *
 * \date Mar 18, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxPluginAlgorithm_EXPORT FilterPresetWidget : public PresetWidget
{
	Q_OBJECT

public:
	FilterPresetWidget(QWidget* parent);
	virtual ~FilterPresetWidget(){};

	void setFilter(FilterPtr filter); ///< sets which filter to operate on

public slots:
	virtual void saveSlot(); ///< called when user tries to save a filter preset
	virtual void deleteSlot(); ///< called when user tries to delete a filter preset

private:
	FilterPtr mFilter;
};

} /* namespace cx */
#endif /* CXFILTERPRESETWIDGET_H_ */
