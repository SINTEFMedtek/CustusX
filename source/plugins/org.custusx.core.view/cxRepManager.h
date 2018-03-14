/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

class Reporter;

/**
 * \file
 * \addtogroup org_custusx_core_view
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
};


/**
 * @}
 */
} //namespace cx

#endif /* CXREPMANAGER_H_ */
