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

#ifndef CXUTILHELPERS_H_
#define CXUTILHELPERS_H_

#include <sstream>
#include <iomanip>
#include <QString>

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

/**stream the range |begin,end> to the ostream.
 * insert separator between each element..
 */
template<class ITER> std::ostream& stream_range(std::ostream& s, ITER begin, ITER end, char separator=' ')
{
	if (begin==end)
		return s;

	std::ostringstream ss; // avoid changing state of input stream
	ss << std::setprecision(3) << std::fixed;

	ss << std::setw(10) << *begin;
	++begin;
	for (; begin!=end; ++begin)
		ss << separator << std::setw(10) << *begin;

	s << ss.str();

	return s;
}
// --------------------------------------------------------

double constrainValue(double val, double min, double max);
int sign(double x);
QString changeExtension(QString name, QString ext);
void sleep_ms(int ms);

/**
 * \}
 */

} // namespace cx

#endif /*CXUTILHELPERS_H_*/
