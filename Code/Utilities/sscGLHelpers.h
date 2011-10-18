#ifndef SSCGLHELPERS_H_
#define SSCGLHELPERS_H_

#define report_gl_error() fgl_really_report_gl_errors(__FILE__, __LINE__, 0)
#define report_gl_error_text(text) fgl_really_report_gl_errors(__FILE__, __LINE__, text)


#ifdef WIN32
	void fgl_really_report_gl_errors( const char *file, int line, const char* text ) {}
#else
	void fgl_really_report_gl_errors( const char *file, int line, const char* text );
#endif //WIN32


#endif /*SSCGLHELPERS_H_*/
