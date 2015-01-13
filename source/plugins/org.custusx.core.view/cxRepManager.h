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

#ifndef CXREPMANAGER_H_
#define CXREPMANAGER_H_

#include "org_custusx_core_view_Export.h"

#include <QObject>
#include <map>
#include <vector>
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class Rep> RepPtr;
}

namespace cx
{

typedef std::map<QString, RepPtr> RepMap;
typedef std::map<QString, VolumetricBaseRepPtr> VolumetricRepMap;
typedef boost::shared_ptr<class ThresholdPreview> ThresholdPreviewPtr;

class Reporter;

/**
 * \file
 * \addtogroup cx_service_visualization
 * @{
 */

/**
 * \brief Rep caching and utilities.
 *
 *  - Utility functions for accessing reps,
 *  - cache for reusing the expensive volumetric reps.
 *
 * \date Dec 10, 2008
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class org_custusx_core_view_EXPORT RepManager: public QObject
{
Q_OBJECT

public:
	static RepManager* getInstance(); ///< get the only instance of this class
	static void destroyInstance(); ///< destroy the only instance of this class

	ThresholdPreviewPtr getThresholdPreview(); ///< Get the ThresholdPreview object

	/**Get a previously cached Rep.
	 *
	 * Optionally, provide an uid that can be used
	 * to cache several different versions of a rep type.
	 *
	 * Name is the name to use IF a new rep is created.
	 *
	 * Not implemented:
	 * If a Rep of the requested type exist but is not used
	 * externally (i.e. has a refcount of 1), it will be reused.
	 * Otherwise, a new rep will be created and returned.
	 */
	template<class REP>
	boost::shared_ptr<REP> getCachedRep(QString uid = "")
	{
		// look for existing value:
		for (RepMultiMap::iterator iter = mRepCache.begin(); iter != mRepCache.end(); ++iter)
		{
			if (iter->first != uid)
				continue;
			int uc = iter->second.use_count();
			if (uc >1)
			{
//				std::cout << "cached rep in use: " << uid.toStdString() << ", use count: " << uc << std::endl;
				continue;
			}

			boost::shared_ptr<REP> retval = boost::dynamic_pointer_cast<REP>(iter->second);
			if (retval)
			{
//				std::cout << "reusing cached rep: " << uid.toStdString() << ", use count: " << uc << std::endl;
				return retval;
			}
		}

		// create new value, store and return:
		boost::shared_ptr<REP> retval = REP::New(uid);
		mRepCache.insert(std::make_pair(uid, retval));

		return retval;
	}

protected:
	static RepManager* mTheInstance; ///< the only instance of this class

	typedef std::multimap<QString, RepPtr> RepMultiMap;
	RepMultiMap mRepCache;

private:
	RepManager(); ///< creates a pool of reps
	virtual ~RepManager(); ///< empty
	RepManager(RepManager const&); ///< not implemented
	RepManager& operator=(RepManager const&); ///< not implemented
	ThresholdPreviewPtr mThresholdPreview; ///< Preview a volume with a selected threshold
};


/**
 * @}
 */
} //namespace cx

#endif /* CXREPMANAGER_H_ */
