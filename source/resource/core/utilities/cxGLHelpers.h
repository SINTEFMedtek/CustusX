/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
