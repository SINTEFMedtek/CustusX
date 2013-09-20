#ifndef HackTPSTransform_hxx
#define HackTPSTransform_hxx

#include "vtkThinPlateSplineTransform.h"

class HackTPSTransform: vtkThinPlateSplineTransform
{
  
public:

    void New();
    virtual void Delete();

    const double* const* GetWarpMatrix(){return MatrixW;}
    vtkPoints* GetSourceLandmarks(){return SourceLandmarks;}
};

#endif
