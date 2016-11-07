Import Overview {#import_overview}
===========================================================

CustusX can import the following data types from file

Image / volume data:
| Input datatype          | Allowed variants                            | Description                                        |
| ----------------------- | -----------------------------------------   | -------------------------------------------------- |
| DICOM                   | 8, 16, or 32 bit (signed or unsigned)       |                                                    |
| .mhd                    | VTK mhd format (with some CX additions)     | https://itk.org/Wiki/ITK/MetaIO/Documentation      |
| .minc                   | VTK minc reader                             | http://www.bic.mni.mcgill.ca/ServicesSoftware/MINC |
| .png                    | VTK png reader                              | 2D pictures                                        |

Polydata / Surface models :
| Input datatype          | Allowed variants                            | Description                                        |
| ----------------------- | -----------------------------               | -------------------------------------------------- |
| .stl                    | VTK stl reader                              |                                                    |
| .vtk                    | VTK mhd format                              | https://itk.org/Wiki/ITK/MetaIO/Documentation      |


The data are stored/exported as:
| Datatype                | Internal format                         | Saved as file                 |
| ----------------------- | -----------------------------           | ----------------------------- |
| Images/volumes          | vtkImageData                            | .mhd                          |
| Surface models/polydata | vtkPolyData                             | .vtk                          |