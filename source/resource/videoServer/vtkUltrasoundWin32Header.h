/*=========================================================================

  Program:   AtamaiUltrasound for VTK
  Module:    $RCSfile: vtkUltrasoundWin32Header.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  \author    $\author dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.2 $

=========================================================================*/
// .NAME vtkUltrasoundWin32Header - manage Windows system differences
// .SECTION Description
// The vtkUltrasoundWin32Header captures some system differences between Unix
// and Windows operating systems.

#ifndef __vtkUltrasoundWin32Header_h
#define __vtkUltrasoundWin32Header_h

//#include <QueensOpenIGTLibsConfigure.h>

#if defined(WIN32) && defined(BUILD_SHARED_LIBS)//&& !defined(QUEENSOPENIGT_STATIC)
#if defined(vtkOpenIGTUltrasound_EXPORTS)
#define VTK_ULTRASOUND_EXPORT __declspec( dllexport )
#else
#define VTK_ULTRASOUND_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_ULTRASOUND_EXPORT
#endif

#endif

