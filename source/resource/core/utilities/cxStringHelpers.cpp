/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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

int convertStringWithDefault(QString text, int def)
{
	bool ok = true;
	int retval = text.toInt(&ok,0);
	if (ok)
		return retval;
	return def;
}


} // namespace cx

