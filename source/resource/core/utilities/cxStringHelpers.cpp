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

#include "cxStringHelpers.h"
#include <QStringList>

namespace cx
{

/** given a text line containing fex [alpha beta "gamm ma" delta]
  * split into a list of ["alpha", "beta" ,"gamm ma", "delta"]
  */
QStringList splitStringContaingQuotes(QString line)
{
	QStringList base = line.split(" ");
	QStringList retval;

	for (int i=0; i<base.size(); ++i)
	{
		if (base[i].startsWith("\"") && !base[i].endsWith("\""))
		{
			QString s;
			do
			{
				s += base[i];
				if ((i+1)<base.size() && !base[i].endsWith("\""))
					s += " ";
				++i;
			} while (i<base.size() && !base[i].endsWith("\""));
			if (i<base.size())
				s += base[i];
			retval.push_back(s);
		}
		else
		{
			retval.push_back(base[i]);
		}

		retval.back() = retval.back().remove("\"");
	}

	retval.removeAll("");
	return retval;
}


} // namespace cx

