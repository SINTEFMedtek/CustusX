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

