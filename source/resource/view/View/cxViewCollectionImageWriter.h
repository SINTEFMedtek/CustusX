/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVIEWCOLLECTIONIMAGEWRITER_H
#define CXVIEWCOLLECTIONIMAGEWRITER_H

#include <QObject>
#include <QPointer>
#include <QMainWindow>
#include "vtkSmartPointer.h"
#include "cxVisServices.h"
#include "cxLayoutData.h"
#include "cxForwardDeclarations.h"

typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;
typedef vtkSmartPointer<class vtkPNGWriter> vtkPNGWriterPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

namespace cx
{
class ViewCollectionWidget;

/** Write the previously rendered contents of the input ViewCollectionWidget
 *  to a vtkImageData.
 *
 */
class cxResourceVisualization_EXPORT ViewCollectionImageWriter
{
public:
	explicit ViewCollectionImageWriter(ViewCollectionWidget* widget);
    vtkImageDataPtr grab();
    static QImage vtkImageData2QImage(vtkImageDataPtr input);
private:
	vtkImageDataPtr view2vtkImageData(ViewPtr view);
	/**
	 * Draw image inside target. pos is given in vtk coordinates inside target.
	 * image is assumed to fit inside target at the indicated position. */
    void drawImageAtPos(vtkImageDataPtr target, vtkImageDataPtr image, QPoint pos);
    /**
     * Get view position in vtk coords, lower left corner*/
    QPoint getVtkPositionOfView(ViewPtr view);
    QPoint qt2vtk(QPoint qpos);

	ViewCollectionWidget* mWidget;
};

} // namespace cx

#endif // CXVIEWCOLLECTIONIMAGEWRITER_H
