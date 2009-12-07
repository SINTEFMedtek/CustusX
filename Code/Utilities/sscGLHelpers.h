#ifndef SSCGLHELPERS_H_
#define SSCGLHELPERS_H_

#define report_gl_error() fgl_really_report_gl_errors(__FILE__, __LINE__)

void fgl_really_report_gl_errors( const char *file, int line );

#endif /*SSCGLHELPERS_H_*/
