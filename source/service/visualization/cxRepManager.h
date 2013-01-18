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

#ifndef CXREPMANAGER_H_
#define CXREPMANAGER_H_

#include <QObject>
#include <map>
#include <set>
#include <vector>
#include "sscForwardDeclarations.h"
#include "sscMessageManager.h"
#include "sscTool.h"
#include "sscTypeConversions.h"
#include "cxThresholdPreview.h"

namespace ssc
{
typedef boost::shared_ptr<class Rep> RepPtr;
typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;
}

namespace cx
{

typedef std::map<QString, ssc::RepPtr> RepMap;
typedef std::map<QString, ssc::VolumetricRepPtr> VolumetricRepMap;

class MessageManager;

/**
 * \file
 * \addtogroup cxServiceVisualization
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
class RepManager: public QObject
{
Q_OBJECT

public:
	static RepManager* getInstance(); ///< get the only instance of this class
	static void destroyInstance(); ///< destroy the only instance of this class

	ThresholdPreviewPtr getThresholdPreview(); ///< Get the ThresholdPreview object

	template<class REP>
	static boost::shared_ptr<REP> findFirstRep(std::vector<ssc::RepPtr> reps, ssc::ToolPtr tool)
	{
		for (unsigned i = 0; i < reps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::shared_dynamic_cast<REP>(reps[i]);
			if (rep && rep->hasTool(tool))
			{
				return rep;
			}
		}
		return boost::shared_ptr<REP>();
	}

	template<class REP>
	static boost::shared_ptr<REP> findFirstRep(std::vector<ssc::RepPtr> reps, ssc::DataPtr data)
	{
		for (unsigned i = 0; i < reps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::shared_dynamic_cast<REP>(reps[i]);
			if (rep && rep->hasData(data))
				return rep;
		}
		return boost::shared_ptr<REP>();
	}

	template<class REP>
	static boost::shared_ptr<REP> findFirstRep(std::vector<ssc::RepPtr> reps)
	{
		for (unsigned i = 0; i < reps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::shared_dynamic_cast<REP>(reps[i]);
			if (rep)
				return rep;
		}
		return boost::shared_ptr<REP>();
	}

	/**Get a volumetric rep based on which image you want to  display.
	 * This is useful when creating the rep is expensive and should be done only once.
	 */
	ssc::VolumetricRepPtr getVolumetricRep(ssc::ImagePtr image);

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
	boost::shared_ptr<REP> getCachedRep(QString uid = "", QString name = "USE_UID")
	{
		// look for existing value:
		for (RepMultiMap::iterator iter = mRepCache.begin(); iter != mRepCache.end(); ++iter)
		{
			if (iter->first != uid)
				continue;
			boost::shared_ptr<REP> retval = boost::shared_dynamic_cast<REP>(iter->second);
			if (retval)
			{
//  		  std::cout << "reusing cached rep: " << uid << std::endl;
				return retval;
			}
		}

		if (name == "USE_UID")
			name = uid;

		// create new value, store and return:
		boost::shared_ptr<REP> retval = REP::New(uid, name);
		mRepCache.insert(std::make_pair(uid, retval));
//	  std::cout << "created new cached rep: " << uid << std::endl;

		return retval;
	}

protected slots:
	void volumeRemovedSlot(QString uid);

protected:

	static RepManager* mTheInstance; ///< the only instance of this class

	VolumetricRepMap mVolumetricRepByImageMap; ///< used for caching reps based on image content
	bool mIsUsingGPU3DMapper;
	double mMaxRenderSize;

//  ssc::OrientationAnnotation3DRepPtr mAnnotationMarker;
//  typedef std::multimap<QString, ssc::RepPtr> RepMultiMap;
	typedef std::multimap<QString, ssc::RepPtr> RepMultiMap;
	RepMultiMap mRepCache;
//  RepMap              mRepMap; ///< contains all the reps in the specific maps above. Use for simplified access.
	void purgeVolumetricReps();

private:
	RepManager(); ///< creates a pool of reps
	virtual ~RepManager(); ///< empty
	RepManager(RepManager const&); ///< not implemented
	RepManager& operator=(RepManager const&); ///< not implemented
	ThresholdPreviewPtr mThresholdPreview; ///< Preview a volume with a selected threshold
};

//RepManager* repManager();

/**
 * @}
 */
} //namespace cx

#endif /* CXREPMANAGER_H_ */
