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
#include <QDomElement>
#include <vtkSmartPointer.h>
#include "cxProperty.h"
#include <memory>



#define ReconstructionMethodService_iid "cx::ReconstructionMethodService"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{
typedef std::shared_ptr<class DoubleProperty> DoublePropertyPtr;
typedef std::shared_ptr<class BoolProperty> BoolPropertyPtr;
typedef std::shared_ptr<class Image> ImagePtr;
typedef std::shared_ptr<class ProcessedUSInputData> ProcessedUSInputDataPtr;

/**
 * \addtogroup org_custusx_usreconstruction
 * \{
 */

typedef std::shared_ptr<class ReconstructionMethodService> ReconstructionMethodServicePtr;

/**
 * Result of an estimate of the time lag between the images and the tracking data.
 * The lag has the same sign convention as the reconstruction parameter 'Extra Temporal Calib':
 * it is added to the tracking time stamps.
 */
struct TimeLagEstimate
{
	TimeLagEstimate() : valid(false), apply(false), lagMs(0), correlation(0) {}
	bool valid; ///< true if a lag was estimated
	bool apply; ///< true if the lag should replace the time calibration settings (Extra Temporal Calib, Align timestamps)
	double lagMs; ///< estimated lag in ms
	double correlation; ///< quality of the estimate
};

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

	/**
	 * Optional: Return true if the algorithm should be asked to estimate the time lag between images and tracking
	 * data (estimateTimeLag()) before the reconstruction.
	 */
	virtual bool isTimeLagEstimateRequested(QDomElement settings) { Q_UNUSED(settings); return false; }
	/**
	 * Optional: Estimate the time lag between images and tracking data.
	 * Called with data that is created without any time calibration, i.e. with the tracking timestamps as recorded.
	 * If the returned estimate is valid and has apply=true, the preprocessor applies the lag to the tracking data
	 * (replacing the user time calibration), and the reconstruction is done with the lagged tracking data.
	 */
	virtual TimeLagEstimate estimateTimeLag(ProcessedUSInputDataPtr input, QDomElement settings) { Q_UNUSED(input); Q_UNUSED(settings); return TimeLagEstimate(); }
};

/**
 * \}
 */

}//namespace cx
Q_DECLARE_INTERFACE(cx::ReconstructionMethodService, ReconstructionMethodService_iid)

#endif //CXRECONSTRUCTIONMETHODSERVICE_H_
