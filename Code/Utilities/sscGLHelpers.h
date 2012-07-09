#ifndef SSCGLHELPERS_H_
#define SSCGLHELPERS_H_

#include <vtkgl.h>

#define GL_TRACE(string) if (vtkgl::StringMarkerGREMEDY) {vtkgl::StringMarkerGREMEDY(0, QString("%1:%2 - %3").arg(__func__).arg(__LINE__).arg(string).toUtf8().constData());}
	
#define report_gl_error() really_report_gl_errors(__FILE__, __LINE__, 0)
#define report_gl_error_text(text) really_report_gl_errors(__FILE__, __LINE__, text)

#ifdef WIN32
	static void really_report_gl_errors( const char *file, int line, const char* text ) {}
#else
	void really_report_gl_errors( const char *file, int line, const char* text );
#endif //WIN32


#endif /*SSCGLHELPERS_H_*/
