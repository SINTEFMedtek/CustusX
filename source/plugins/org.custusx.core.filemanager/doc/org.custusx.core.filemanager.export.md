Export Overview {#export_overview}
===========================================================

All data for a session is stored in a "Patient folder" ending with .cx3:

<pre>
.cx3
 |- Images         - Images and polydata
 |- Logs           - Log files and position data
 |- Screenshots    - Screenshots (still and video)
 |- US_Acq (*)     - 3D ultrasound acquisition data
</pre>

\addindex export_data_widget
Export Widget {#export_data_widget}
===========================================================
Instead of copying out files from the "Patient folder", the export widget can be used for exporting data in a few formats.
1. Choose desired export format.
2. Select one or all data of this type.
3. Press export button.


File types used by CustusX {#export_file_types}
===========================================================

Some data are created by CustusX or imported through plugins, while others are imported directly (\ref import_overview)
| Datatype                              | Saved/exported as file                  |
| -----------------------               | -----------------------------           |
| Images/volumes                        | .mhd                                    |
| Surface models/polydata               | .vtk (Can be exported as .stl or .cgeo) |
| Tracking positions                    | .snwpos                                 |
| Grabbed ultrasound/video streams (*)  | .mhd                                    |
| Digital 2D/3D ultrasound streams (*)  | .mhd                                    |
| Metrics                               | Can be exported as .txt                 |
| Screenshots                           | .png                                    |
| Video screeshots (uses VLC)           | .mp4                                    |

(*) For file format of 3D ultrasound acquisitions, see \ref org_custusx_resource_core_usacquisitionfileformat
