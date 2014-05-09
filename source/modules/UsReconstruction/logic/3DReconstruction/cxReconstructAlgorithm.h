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

#ifndef CXRECONSTRUCTALGORITHM_H_
#define CXRECONSTRUCTALGORITHM_H_

#include <vector>
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxDataAdapter.h"

class QDomElement;

#define ReconstructionService_iid "cx::ReconstructionService"

namespace cx
{

typedef boost::shared_ptr<class ProcessedUSInputData> ProcessedUSInputDataPtr;

/**
 * \addtogroup cx_module_usreconstruction
 * \{
 */

typedef boost::shared_ptr<class ReconstructionService> ReconstructionServicePtr;

/**
 * \brief Abstract interface for reconstruction algorithm.
 *
 * Subclass in order to implement a specific reconstruction.
 *
 *  \date June 23, 2010
 *  \author Ole Vegard Solberg
 *  \author Janne Beate Bakeng
 */
class ReconstructionService //: public QObject
{
    //Q_OBJECT
public:
	virtual ~ReconstructionService() {}
	virtual QString getName() const = 0;
	/**
	 *  Fill settings for this algorithm.
	 *  Input is the root node for this algo, filled with stored settings (if any).
	 *  On completion, the root is filled with default values for settings.
	 */
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root) = 0;
	/**
	 * \param frameInfo Timetags and positions for the input frames
	 * \param frameData The frame data. Assumes that the transfrom is identity.
	 * \param outputData [Out] The reconstructed volume. Memory must be allocated in advance.
	 * \param frameMask A mask image for the frame.
	 * \param settings Reference to settings file containing algorithm-specific settings
	 */
	virtual bool reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings) = 0;
};

/**
 * \}
 */

}//namespace cx
Q_DECLARE_INTERFACE(cx::ReconstructionService, ReconstructionService_iid)

#endif //CXRECONSTRUCTALGORITHM_H_
