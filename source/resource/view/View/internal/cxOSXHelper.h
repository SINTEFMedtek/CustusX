/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXOSXHELPER_H_
#define CXOSXHELPER_H_

namespace cx
{
/**
 * Disable Retina resolution in VTK widgets, enabling
 * the main application to run in retina while VTK does not.
 *
 * Remove this as soon as retina support is added to VTK.
 *
 * Based on http://public.kitware.com/pipermail/vtkusers/2015-February/090117.html
 */
#ifdef CX_APPLE
void disableGLHiDPI( long a_id );
#else
static void disableGLHiDPI( long a_id ) {}
#endif
} // namespace cx


#endif /* CXOSXHELPER_H_ */
