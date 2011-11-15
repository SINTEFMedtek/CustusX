/*
 *  identify.h
 *  DICOMLib
 *
 *  Created by Per M on 07/3/7.
 *  Copyright 2007 MISON AS. All rights reserved.
 *
 */

#ifndef DICOMLIB_IDENTIFY_H
#define DICOMLIB_IDENTIFY_H

#include "DICOMLib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define swap16(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
                   (((uint16_t)(A) & 0x00ff) << 8))
#define swap32(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                   (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                   (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                   (((uint32_t)(A) & 0x000000ff) << 24))

bool identify( const char *path );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
