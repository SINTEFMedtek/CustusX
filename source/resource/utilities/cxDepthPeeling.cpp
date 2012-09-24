/*
 * cxDepthPeeling.cpp
 *
 * Copied from http://www.vtk.org/Wiki/VTK/Examples/Cxx/Visualization/CorrectlyRenderingTranslucentGeometry
 *
 *  Created on: Sep 11, 2012
 *      Author: olevs
 */

#include "cxDepthPeeling.h"
#include "sscMessageManager.h"

/**
 * Generate a bunch of overlapping spheres within one poly data set:
 * one big sphere evenly surrounded by four small spheres that intersect the
 * centered sphere.
 * @param theta sphere sampling resolution (THETA)
 * @param phi sphere sampling resolution (PHI)
 * @return the set of spheres within one logical poly data set
 **/
vtkSmartPointer<vtkAppendPolyData> GenerateOverlappingBunchOfSpheres(int theta, int phi)
{
  vtkSmartPointer<vtkAppendPolyData> appendData =
    vtkSmartPointer<vtkAppendPolyData>::New();

  for (int i = 0; i < 5; i++)
    {
    vtkSmartPointer<vtkSphereSource> sphereSource =
      vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetThetaResolution(theta);
    sphereSource->SetPhiResolution(phi);
    sphereSource->SetRadius(0.5); // all spheres except the center
                                  // one should have radius = 0.5
    switch (i)
      {
      case 0:
        sphereSource->SetRadius(1);
        sphereSource->SetCenter(0, 0, 0); break;
      case 1:
        sphereSource->SetCenter(1, 0, 0); break;
      case 2:
        sphereSource->SetCenter(-1, 0, 0); break;
      case 3:
        sphereSource->SetCenter(0, 1, 0); break;
      case 4:
        sphereSource->SetCenter(0, -1, 0); break;
      }
    sphereSource->Update();
    appendData->AddInputConnection(sphereSource->GetOutputPort());
    }

  return appendData;
}

/**
 * Setup the rendering environment for depth peeling (general depth peeling
 * support is requested).
 * @see IsDepthPeelingSupported()
 * @param renderWindow a valid openGL-supporting render window
 * @param renderer a valid renderer instance
 * @param maxNoOfPeels maximum number of depth peels (multi-pass rendering)
 * @param occulusionRation the occlusion ration (0.0 means a perfect image,
 * >0.0 means a non-perfect image which in general results in faster rendering)
 * @return TRUE if depth peeling could be set up
 */
bool SetupEnvironmentForDepthPeeling(
		vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer, int maxNoOfPeels,
		double occlusionRatio)
{
  if (!renderWindow || !renderer)
  {
	  ssc::messageManager()->sendWarning("Can't set depth peeling. No render / renderwindow");
	  return false;
  }

  // 1. Use a render window with alpha bits (as initial value is 0 (false)):
  renderWindow->SetAlphaBitPlanes(true);

  // 2. Force to not pick a framebuffer with a multisample buffer
  // (as initial value is 8):
  renderWindow->SetMultiSamples(0);

  // 3. Choose to use depth peeling (if supported) (initial value is 0 (false)):
  renderer->SetUseDepthPeeling(true);

  // 4. Set depth peeling parameters
  // - Set the maximum number of rendering passes (initial value is 4):
  renderer->SetMaximumNumberOfPeels(maxNoOfPeels);
  // - Set the occlusion ratio (initial value is 0.0, exact image):
  renderer->SetOcclusionRatio(occlusionRatio);

  return true;
}

bool TurnOffDepthPeeling(
		vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer)
{
  if (!renderWindow || !renderer)
  {
	  ssc::messageManager()->sendWarning("Can't turn off depth peeling. No render / renderwindow");
	  return false;
  }

  // Set values back to defaults
  // TODO: Save defaults (see IsDepthPeelingSupported())
  renderWindow->SetAlphaBitPlanes(false);
  renderWindow->SetMultiSamples(8);
  renderer->SetUseDepthPeeling(false);
  renderer->SetMaximumNumberOfPeels(4);
  renderer->SetOcclusionRatio(0.0);

  return true;
}

/**
 * Find out whether this box supports depth peeling. Depth peeling requires
 * a variety of openGL extensions and appropriate drivers.
 * @param renderWindow a valid openGL-supporting render window
 * @param renderer a valid renderer instance
 * @param doItOffscreen do the test off screen which means that nothing is
 * rendered to screen (this requires the box to support off screen rendering)
 * @return TRUE if depth peeling is supported, FALSE otherwise (which means
 * that another strategy must be used for correct rendering of translucent
 * geometry, e.g. CPU-based depth sorting)
 */
bool IsDepthPeelingSupported(vtkSmartPointer<vtkRenderWindow> renderWindow,
		vtkSmartPointer<vtkRenderer> renderer, bool doItOffScreen)
{
  if (!renderWindow || !renderer)
    {
	  ssc::messageManager()->sendWarning("Can't test depth peeling. No render / renderwindow");
	  return false;
    }

  bool success = true;

  // Save original renderer / render window state
  bool origOffScreenRendering = renderWindow->GetOffScreenRendering() == 1;
  bool origAlphaBitPlanes = renderWindow->GetAlphaBitPlanes() == 1;
  int origMultiSamples = renderWindow->GetMultiSamples();
  bool origUseDepthPeeling = renderer->GetUseDepthPeeling() == 1;
  int origMaxPeels = renderer->GetMaximumNumberOfPeels();
  double origOcclusionRatio = renderer->GetOcclusionRatio();

  // Activate off screen rendering on demand
  renderWindow->SetOffScreenRendering(doItOffScreen);

  // Setup environment for depth peeling (with some default parametrization)
  success = success && SetupEnvironmentForDepthPeeling(renderWindow, renderer,
                                                       100, 0.1);

  // Do a test render
  renderWindow->Render();

  // Check whether depth peeling was used
  success = success && renderer->GetLastRenderingUsedDepthPeeling();

  // recover original state
  renderWindow->SetOffScreenRendering(origOffScreenRendering);
  renderWindow->SetAlphaBitPlanes(origAlphaBitPlanes);
  renderWindow->SetMultiSamples(origMultiSamples);
  renderer->SetUseDepthPeeling(origUseDepthPeeling);
  renderer->SetMaximumNumberOfPeels(origMaxPeels);
  renderer->SetOcclusionRatio(origOcclusionRatio);

  return success;
}
