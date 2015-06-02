US Acquisition Data File Format {#org_custusx_resource_core_usacquisitionfileformat}
===================

File Format Description {#org_custusx_resource_core_usacquisitionfileformat_overview}
===========================================================

This file format is used to store one US acquisition operation, i.e. storing a
US image stream along with tracking and probe information.

The acquisition is available from \ref org_custusx_acquisition_widgets_acquisition.

All files describing one acquisition lie the same folder.  The files all have
the name format US-Acq_{index}\_{TS}{stream}.{type},
where
- {index} is a running index, for convenience.
- {TS} is a timestamp
- {stream} is the uid of the video stream.
- {type} is the format of that specific file.

Together, the files contains information about the us images and their
timestamps, the tracking positions and their timestamps, and the probe
calibration.

In the following, we use {filebase} = US-Acq_{index}_{TS}{stream}.


Frame Data {filebase}_{frame}.mhd {#us_acq_file_format_mhd_indexed}
-----------------------------------------------------------

A sequence of files in the metaheader file format containing the image data,
one file for each frame. The frame index is given by the index {frame} in the
file name.

The metaheader files contains orientation+position info identical to the
positions in \ref us_acq_file_format_fp. The files can thus be imported
directly back into CustusX and appear in the correct position.

See http://www.itk.org/Wiki/MetaIO/Documentation for more.

Replaces \ref us_acq_file_format_mhd.


Profile Definition {filebase}.probedata.xml {#us_acq_file_format_file_probedata}
-----------------------------------------------------------

This file contains the [probe definition](#cx_us_probe_definition). Replaces \ref us_acq_file_format_file_xml .


Frame Timestamps {filebase}.fts {#us_acq_file_format_file_fts}
-----------------------------------------------------------

This file contains the frame timestamps. This is a sequence of
newline-separated floating-point numbers in *milliceconds*. The starting point
is irrelevant. The number of timestamps must equal the number of us frames.


Frame Positions {filebase}.fp {#us_acq_file_format_fp}
-----------------------------------------------------------

This file contains the frame positions, one matrix for each US frame.
Each matrix is the `rMu`,
i.e. the transform from lower-left centered image space to
global reference (See \ref coordinate_systems and \ref cx_us_probe_definition).

This is a newline-separated sequence of matrices, one for each position.
The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix
numbers is whitespace-separated with newline between rows. Thus the number of
lines in this file is (# tracking positions) x 3.


Tracking Timestamps {filebase}.tts {#us_acq_file_format_tts}
-----------------------------------------------------------

This file contains the tracking timestamps. The format equals
\ref us_acq_file_format_file_fts, but the number of timestamps equals the
number of tracking positions.


Tracking Positions {filebase}.tp {#us_acq_file_format_tp}
-----------------------------------------------------------

This file contains the tracking positions, one matrix for each tracking sample.
Each matrix is the `prMt`, i.e. the transform from tool to patient reference
(See \ref coordinate_systems ).

This is a newline-separated sequence of matrices, one for each position.
The last line of the matrix (always containing 0 0 0 1) is omitted. The matrix
numbers is whitespace-separated with newline between rows. Thus the number of
lines in this file is (# tracking positions) x 3.


Image Mask {filebase}.mask.mhd {#us_acq_file_format_mask}
-----------------------------------------------------------

This file contains the image mask. The binary image shows what parts
of the frame images contain valid US data. This file is only written,
not read. It can be constructed from the probe data.

Obsolete files
-----------------------------------------------------------

### {filebase}.mhd {#us_acq_file_format_mhd}
*obsolete*

Used prior to version cx3.4.0.

A file in the metaheader file format containing the uncompressed image data.
the z-direction is the time axis, i.e. the z dim is the number of us frames.
See http://www.itk.org/Wiki/MetaIO/Documentation for more.

Two extra tags are added:

  ConfigurationID = \<path:inside:ProbeCalibConfigs.xml\>
  ProbeCalibration = \<not used\>

The ConfigurationID refers to a specific configuration within
ProbeCalibConfigs.xml, using colon separators.


### ProbeCalibConfigs.xml {#us_acq_file_format_file_xml}
*obsolete*

This file contains the probe definition, and is copied from the
config/tool/Tools folder.

