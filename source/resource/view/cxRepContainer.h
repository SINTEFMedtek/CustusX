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

#ifndef CXREPCONTAINER_H
#define CXREPCONTAINER_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <map>
#include <vector>
#include "cxForwardDeclarations.h"

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
//	template<class REP>
//	static boost::shared_ptr<REP> findFirstRep(std::vector<RepPtr> reps, ToolPtr tool)
//	{
//		for (unsigned i = 0; i < reps.size(); ++i)
//		{
//			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(reps[i]);
//			if (rep && rep->hasTool(tool))
//			{
//				return rep;
//			}
//		}
//		return boost::shared_ptr<REP>();
//	}

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
//	template<class REP>
//	static boost::shared_ptr<REP> findFirstRep(std::vector<RepPtr> reps, DataPtr data)
//	{
//		for (unsigned i = 0; i < reps.size(); ++i)
//		{
//			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(reps[i]);
//			if (rep && rep->hasData(data))
//				return rep;
//		}
//		return boost::shared_ptr<REP>();
//	}

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
//	template<class REP>
//	static boost::shared_ptr<REP> findFirstRep(std::vector<RepPtr> reps)
//	{
//		for (unsigned i = 0; i < reps.size(); ++i)
//		{
//			boost::shared_ptr<REP> rep = boost::dynamic_pointer_cast<REP>(reps[i]);
//			if (rep)
//				return rep;
//		}
//		return boost::shared_ptr<REP>();
//	}

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

private:
	std::vector<RepPtr> mReps;
};


} //namespace cx

#endif // CXREPCONTAINER_H
