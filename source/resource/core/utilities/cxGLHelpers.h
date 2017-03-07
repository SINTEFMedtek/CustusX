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

#ifndef CXGLHELPERS_H_
#define CXGLHELPERS_H_


/**
 * \addtogroup cx_resource_core_utilities
 * @{
 */

//#define GL_TRACE(string) if (vtkgl::StringMarkerGREMEDY) {vtkgl::StringMarkerGREMEDY(0, QString("%1:%2 - %3").arg(__func__).arg(__LINE__).arg(string).toUtf8().constData());}

/** check for GL errors.
 *
 * NOTE: Only call this function when a valid context exist
 * (i.e vtkRenderWindow::GetNeverRendered() == false)
 */
#define report_gl_error() really_report_gl_errors(__FILE__, __LINE__, 0)
#define report_gl_error_text(text) really_report_gl_errors(__FILE__, __LINE__, text)

#ifdef WIN32
	static void really_report_gl_errors( const char *file, int line, const char* text ) {}
#else
	void really_report_gl_errors( const char *file, int line, const char* text );
#endif //WIN32

/**
 * @}
 */

#endif /*CXGLHELPERS_H_*/
