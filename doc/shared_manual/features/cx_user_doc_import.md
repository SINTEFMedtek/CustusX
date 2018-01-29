Import Overview {#import_overview}
===========================================================


Import file types {#import_file_types}
===========================================================

CustusX can import the following data types from file.

Image / volume data:
| Input datatype          | Allowed variants                            | Description                                        |
| ----------------------- | -----------------------------------------   | -------------------------------------------------- |
| DICOM                   | 8, 16, or 32 bit (signed or unsigned)       | http://dicom.nema.org/standard.html                |
| .mhd                    | VTK mhd reader (with some CX additions)     | https://itk.org/Wiki/ITK/MetaIO/Documentation      |
| .png                    | VTK png reader                              | 2D pictures                                        |


Polydata / Surface models :
| Input datatype          | Allowed variants                            | Description                                        |
| ----------------------- | -----------------------------               | -------------------------------------------------- |
| .stl                    | VTK stl reader                              |                                                    |
| .vtk                    | VTK vtk format                              | www.vtk.org/VTK/img/file-formats.pdf               |
| .vtp                    | VTK vtp format                              | www.vtk.org/VTK/img/file-formats.pdf               |
