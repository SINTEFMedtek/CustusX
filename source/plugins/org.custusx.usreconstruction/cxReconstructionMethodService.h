/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
