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


#ifndef CXRECONSTRUCTIONMETHODSERVICE_H_
#define CXRECONSTRUCTIONMETHODSERVICE_H_

#include "org_custusx_usreconstruction_Export.h"

#include <vector>
#include <QObject>
#include <vtkSmartPointer.h>
#include "cxProperty.h"
#include  "boost/shared_ptr.hpp"


class QDomElement;

#define ReconstructionMethodService_iid "cx::ReconstructionMethodService"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{
typedef boost::shared_ptr<class DoubleProperty> DoublePropertyPtr;
typedef boost::shared_ptr<class BoolProperty> BoolPropertyPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class ProcessedUSInputData> ProcessedUSInputDataPtr;

/**
 * \addtogroup org_custusx_usreconstruction
 * \{
 */

typedef boost::shared_ptr<class ReconstructionMethodService> ReconstructionMethodServicePtr;

/**
 * \brief Abstract interface for reconstruction algorithm.
 *
 * Subclass in order to implement a specific reconstruction.
 *
 *  \date June 23, 2010
 *  \author Ole Vegard Solberg
 *  \author Janne Beate Bakeng
 */
class org_custusx_usreconstruction_EXPORT ReconstructionMethodService : public QObject
{
    Q_OBJECT
public:
	virtual ~ReconstructionMethodService() {}
	virtual QString getName() const = 0;
	/**
	 *  Fill settings for this algorithm.
	 *  Input is the root node for this algo, filled with stored settings (if any).
	 *  On completion, the root is filled with default values for settings.
	 */
	virtual std::vector<PropertyPtr> getSettings(QDomElement root) = 0;
	/**
	 * \param input data to process
	 * \param outputData [Out] The reconstructed volume. Memory must be allocated in advance.
	 * \param settings Reference to settings file containing algorithm-specific settings
	 */
	virtual bool reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings) = 0;
};

/**
 * \}
 */

}//namespace cx
Q_DECLARE_INTERFACE(cx::ReconstructionMethodService, ReconstructionMethodService_iid)

#endif //CXRECONSTRUCTIONMETHODSERVICE_H_
