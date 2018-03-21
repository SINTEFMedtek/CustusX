/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "matrixInterpolation.h"
//#include <vector>
//#include "itkArray.h"
//#include "itkArray2D.h"
#include <iostream>
//#include "vnl/vnl_matrix.h"
//#include "vnl/vnl_vector.h"
//typedef vnl_matrix<double> vnl_matrix_double;


std::vector<vnl_matrix_double> matrixInterpolation(vnl_vector<double> DataPoints,
    std::vector<vnl_matrix_double> DataValues, vnl_vector<double> InterpolationPoints, std::string InterpolationMethod)
{
  try
  {
    if (DataPoints.size() != DataValues.size())
    {
      std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n" << ">>>>>>>> In "
          << "::Number of input data points differs from number of input data values!!!  Throw up ...\n";
      throw;
    }

    std::vector<vnl_matrix_double> InterpolationData(InterpolationPoints.size());

    if (InterpolationMethod.compare("closest point") == 0) // Closest point "interpolation"
    {
      unsigned int i = 1;
      for (unsigned int j = 0; j < InterpolationPoints.size(); j++)
      {
        while (DataPoints[i] < InterpolationPoints[j] && i < DataPoints.size() - 1)
        {
          i++;
        }

        if (std::abs(DataPoints[i - 1] - InterpolationPoints[j]) < std::abs(DataPoints[i] - InterpolationPoints[j]))
        {
          InterpolationData[j] = DataValues[i - 1];
        }
        else
        {
          InterpolationData[j] = DataValues[i];
        }
      }

    }

    else if (InterpolationMethod.compare("linear") == 0) // Linear interpolation
    {
      unsigned int j = 0;
      for (unsigned int i = 0; i < InterpolationPoints.size(); i++)
      {
        while (DataPoints.get(j + 1) < InterpolationPoints.get(i) && j + 1 < DataPoints.size() - 1)
        {
          j++;
        }
        double t = (InterpolationPoints.get(i) - DataPoints.get(j)) / (DataPoints.get(j + 1) - DataPoints.get(j));
//        std::cout << "t= " << t << std::endl;

        // Translation component interpolation
        // -----------------------------------------------

        vnl_vector<double> InterpolatedTranslationComponent(4);
        for (int k = 0; k < 3; k++)
        {
          InterpolatedTranslationComponent.put(k, (1 - t) * DataValues.at(j).get(k, 3) + t * DataValues.at(j + 1).get(
              k, 3));
        }
//        std::cout << DataValues.at(j).get(0, 3) << " " << DataValues.at(j).get(1, 3) << " " << DataValues.at(j).get(2, 3) << std::endl;
        InterpolatedTranslationComponent.put(3, 1);

        // Rotation matrix interpolation
        // -----------------------------------------------
        // This procedure is taken from Eberly (2008), "Rotation
        // Representations and Performance Issues" found at
        // http://www.geometrictools.com/.

        // Step 1. Extract the rotational parts of the
        //         transformation matrix and compute a matrix R.

        vnl_matrix<double> P = DataValues.at(j).extract(3, 3);
        vnl_matrix<double> Q = DataValues.at(j + 1).extract(3, 3);
        vnl_matrix<double> R = P.transpose() * Q;

        // Step 2. Compute an axis-angle representation of R.

        vnl_vector<double> A(3, 0);
        double Argument = (R.get(0, 0) + R.get(1, 1) + R.get(2, 2) - 1) / 2;
        // Due to roundoff error, the argument can become
        // slightly larger than 1, causing an invalid input to
        // acos. In these cases, it is assumed that the rotation
        // is negligable, and the argument is set to 1 (making
        // theta 0).
        if (Argument > 1)
        {
          Argument = 1;
        }
        double theta = acos(Argument);

        if (theta == 0)
        {
          // There is no rotation, and the vector is set to an
          // arbitrary unit vector ([1 0 0]).
          A.put(0, 1);
        }
        else if (theta < 3.14159265)
        {
          A.put(0, R.get(2, 1) - R.get(1, 2));
          A.put(1, R.get(0, 2) - R.get(2, 0));
          A.put(2, R.get(1, 0) - R.get(0, 1));
          A.normalize();
        }
        else
        {
          if (R.get(0, 0) > R.get(1, 1) && R.get(0, 0) > R.get(2, 2))
          {
            A.put(0, sqrt(R.get(0, 0) - R.get(1, 1) - R.get(2, 2) + 1) / 2);
            A.put(1, R.get(0, 1) / (2 * A.get(0)));
            A.put(2, R.get(0, 2) / (2 * A.get(0)));
          }
          else if (R.get(1, 1) > R.get(0, 0) && R.get(0, 0) > R.get(2, 2))
          {
            A.put(1, sqrt(R.get(1, 1) - R.get(0, 0) - R.get(2, 2) + 1) / 2);
            A.put(0, R.get(0, 1) / (2 * A.get(1)));
            A.put(2, R.get(1, 2) / (2 * A.get(1)));
          }
          else
          {
            A.put(2, sqrt(R.get(2, 2) - R.get(1, 1) - R.get(0, 0) + 1) / 2);
            A.put(1, R.get(1, 2) / (2 * A.get(2)));
            A.put(0, R.get(0, 2) / (2 * A.get(2)));
          }
        }

        // Step 3. Multiply the angle theta by t and convert
        //         back to rotation matrix representation.

        vnl_matrix<double> S(3, 3, 0);
        vnl_matrix<double> I(3, 3);
        vnl_matrix<double> Rt(3, 3);

        S.put(0, 1, -A.get(2));
        S.put(0, 2, A.get(1));
        S.put(1, 0, A.get(2));
        S.put(1, 2, -A.get(0));
        S.put(2, 0, -A.get(1));
        S.put(2, 1, A.get(0));

        I.set_identity();

        Rt = I + sin(t * theta) * S + (1 - cos(t * theta)) * S * S;

        // Step 4. Compute the interpolated matrix, known as the
        //         slerp (spherical linear interpolation).

        vnl_matrix<double> InterpolatedRotationComponent = P * Rt;

        // Transformation matrix composition
        // -----------------------------------------------
        // Compose a 4x4 transformation matrix from the
        // interpolated translation and rotation components.

        InterpolationData.at(i).set_size(4, 4);
        InterpolationData.at(i).fill(0);
        for (int r = 0; r < 3; r++)
        {
          for (int c = 0; c < 3; c++)
          {
            InterpolationData.at(i).put(r, c, InterpolatedRotationComponent.get(r, c));
          }
        }
        InterpolationData.at(i).set_column(3, InterpolatedTranslationComponent);

      }
    }
    return InterpolationData;

  } catch (...)
  {
    std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n" << ">>>>>>>> In "
        << "::Failed to interpolate the given data!!!  Throw up ...\n";
    throw;
  }

}
