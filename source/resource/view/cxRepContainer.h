/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREPCONTAINER_H
#define CXREPCONTAINER_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <map>
#include <vector>
#include "cxForwardDeclarations.h"
#include "cxTool.h"

namespace cx
{
typedef boost::shared_ptr<class Rep> RepPtr;

typedef std::map<QString, RepPtr> RepMap;
typedef std::map<QString, VolumetricBaseRepPtr> VolumetricRepMap;

/**
 *  - Utility functions for accessing reps,
 *
 * \ingroup cx_resource_view
 *
 * \date Dec 10, 2008
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT RepContainer
{
public:
	RepContainer(std::vector<RepPtr> reps) : mReps(reps) {}

	template<class REP>
	boost::shared_ptr<REP> findFirst(ToolPtr tool)
	{
		for (unsigned i = 0; i < mReps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(mReps[i]);
			if (rep && rep->hasTool(tool))
			{
				return rep;
			}
		}
		return boost::shared_ptr<REP>();
	}

	template<class REP>
	boost::shared_ptr<REP> findFirst(DataPtr data)
	{
		for (unsigned i = 0; i < mReps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(mReps[i]);
			if (rep && rep->hasData(data))
				return rep;
		}
		return boost::shared_ptr<REP>();
	}

	template<class REP>
	boost::shared_ptr<REP> findFirst()
	{
		for (unsigned i = 0; i < mReps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(mReps[i]);
			if (rep)
				return rep;
		}
		return boost::shared_ptr<REP>();
	}

	template<class REP>
	static std::vector<boost::shared_ptr<REP> > findReps(std::vector<RepPtr> reps)
	{
		std::vector<boost::shared_ptr<REP> > retval;
		for (unsigned i = 0; i < reps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(reps[i]);
			if (rep)
				retval.push_back(rep);
		}
		return retval;
	}

	template<class REP>
	boost::shared_ptr<REP> findManualToolRep()
	{
		for (unsigned i = 0; i < mReps.size(); ++i)
		{
			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(mReps[i]);
			if (rep && rep->getTool()->hasType(Tool::TOOL_MANUAL))
			{
				return rep;
			}
		}
		return boost::shared_ptr<REP>();
	}

private:
	std::vector<RepPtr> mReps;
};


} //namespace cx

#endif // CXREPCONTAINER_H
