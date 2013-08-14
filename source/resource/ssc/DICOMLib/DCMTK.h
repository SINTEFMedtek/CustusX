/*
 *  DCMTK.h
 *  DICOMLib
 *
 *  Created by Per M on 07/3/2.
 *  Copyright 2007 MISON AS. All rights reserved.
 *
 */
 
#ifndef DICOMLIB_DCMTK_H
#define DICOMLIB_DCMTK_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"

#include "DICOMLib.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * Pass dummy structs into this and it will fill them with whatever useful info we find in
 * the given file.
 */
int parse_DICOM( DcmItem *dset, struct study_t *study, struct series_t *series, struct instance_t *instance, int curFrame );

/** Generate full study list from DICOMDIR. */
int DICOM_Dir( const char *path, struct study_t **study );

/** Generate file list of all series contents for later sorting. */
struct filenode *DICOM_Dir_Nodes( struct study_t *study );

/** Generate automatic window suggestions and range values for the given series and given instance in it. */
int DICOM_image_window_auto( struct series_t *series, struct instance_t *instance );

/** Output image in given size, adjusted for aspect ratio if either x or y is zero. VOI LUT applied. Cached for speed. */
void *DICOM_image_scaled( const struct instance_t *instance, int *x, int *y, int bits, int frame );

int DICOM_Init( void );		///< Initialize codecs for DCMTK
int DICOM_Done( void );		///< Clean up DCMTK

#ifdef __cplusplus
}
#endif

#endif
