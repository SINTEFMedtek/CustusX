Import Overview {#import_overview}
===========================================================

\addindex import_widget
Import Widget {#import_widget}
===========================================================
Import is performed in 3 steps:
1. Add one or multiple files that are to be imported `<Ctrl+I>`.
2. Enter correct parameters for all files.
3. Press import button.

File Parameters
===========================================================
Anatomical coordinate system
* LPS - Used by CustusX and DICOM
* RAS - Used by NIfTI and ITK Snap (Make sure this is selscted when importing NIfTI files, or data from ITK Snap)

Import Parents transform
* Copies the transform from the parent object

Set parent
* Set a parent object. This will connect the two objects, so that they move together when performing operations like registrations.


Import file types {#import_file_types}
===========================================================

CustusX can import the following data types from file.

Image / volume data:
| Input datatype          | Allowed variants                            | Description                                        |
| ----------------------- | -----------------------------------------   | -------------------------------------------------- |
| DICOM                   | 8, 16, or 32 bit (signed or unsigned)       | http://dicom.nema.org/standard.html                |
| .mhd                    | VTK mhd reader (with some CX additions)     | https://itk.org/Wiki/ITK/MetaIO/Documentation      |
| .nii                    | VTK NIfTI reader                            | NIfTI file format                                  |
| .png                    | VTK png reader                              | 2D pictures                                        |


Polydata / Surface models:
| Input datatype          | Allowed variants                            | Description                                        |
| ----------------------- | -----------------------------               | -------------------------------------------------- |
| .stl                    | VTK stl reader                              |                                                    |
| .vtk                    | VTK vtk format                              | www.vtk.org/VTK/img/file-formats.pdf               |
| .vtp                    | VTK vtp format                              | www.vtk.org/VTK/img/file-formats.pdf               |


PointMetrics:
| Input datatype          | Allowed variants                            | Description                                        |
| ----------------------- | -----------------------------               | -------------------------------------------------- |
| .tag                    | VTK MNI tag point reader                    | MNI tag files.                                     |
