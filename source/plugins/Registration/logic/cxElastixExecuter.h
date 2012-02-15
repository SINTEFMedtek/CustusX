// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.


#ifndef CXELASTIXEXECUTER_H_
#define CXELASTIXEXECUTER_H_

#include <QString>
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**
 * \brief ElastiX command-line wrapper.
 *
 * This class wraps a call to the ElastiX library,
 * \ref http://elastix.isi.uu.nl .
 *
 * Call the run method to execute an elastiX registration,
 * then get the results using the getters.
 *
 * The class is single-threaded. Wrap it in a threader,
 * because it might use a lot of time!
 *
 * \todo Add feedback from the command line.
 *
 *
 * \date Feb 4, 2012
 * \author Christian Askeland, SINTEF
 */

class ElastixExecuter
{
public:
	ElastixExecuter();
	virtual ~ElastixExecuter();

	void run(QString application,
	         ssc::ImagePtr fixed,
	         ssc::ImagePtr moving,
	         QString outdir,
	         QStringList parameterfiles);
	ssc::Transform3D getAffineResult() const;
};

/**
 * @}
 */
} /* namespace cx */
#endif /* CXELASTIXEXECUTER_H_ */
