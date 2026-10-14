US Reconstruction Plugin {#org_custusx_usreconstruction}
===================

Overview {#org_custusx_usreconstruction_overview}
========================

Reconstruct 3D US data from acquired a 2D sequence.

\addindex us_reconstruction
US Reconstruction Widget {#org_custusx_usreconstruction_widgets_us_reconstruction}
===========================================================

Reconstruct 3D US data from acquired 2D sequence.

Select a US Reconstruction algorithm from the list in \ref cx_user_doc_group_usreconstruction.

Processed positions
-------------------

When a reconstruction is done, the positions that the reconstruction algorithm used are written next to the input data (the .fts file):

- `<name>.tp.corr`: One position (prMt, same format as the tracking position file .tp) for each frame that was used
- `<name>.tts.corr`: The time stamps (ms) of the positions, same format as the tracking timestamp file .tts

The positions are those after the position filter, the time calibration (including an estimated lag), and any correction done by the algorithm (e.g. the VIC image correlation correction). They are given at the frame times, so if the files are used as `.tp` and `.tts` they reproduce the same reconstruction when Position Filter Strength is 0, no time calibration is used, and image correlation correction is off. Frames that were removed by the preprocessing are not included. When both a B-mode and an angio volume are reconstructed, the positions from the B-mode reconstruction are written.




\defgroup cx_user_doc_group_usreconstruction US Reconstruction Algorithms

All available US Reconstruction Algorithms

