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

Position filter
---------------

The tracker positions can be filtered before they are used. The filter is selected with the *Position Filter* option:

- **Off**: The positions are used as they are.
- **Moving average** (default): Centered moving average over the tracker samples. The window is set by *Position Filter Strength* (0 = no filtering). The filter does not delay the data, but it reduces the amplitude of fast movements.
- **Holt**: Double exponential smoothing (Holt's linear method), which tracks both the level and the trend of the movement. The parameters are:
  - *Holt alpha* (0 to 1): Smoothing of the level. 1 gives no smoothing, a small value gives strong smoothing. Default 0.5.
  - *Holt beta* (0 to 1): Smoothing of the trend (the speed of the movement). Default 0.3.
  - *Holt both directions*: The filter is run both forward and backward in time and the mean of the two is used. This gives no delay of the positions. If turned off, the filter is causal and the filtered positions are delayed relative to the images, which acts as an added time lag. Default on.
  - *Holt add trend*: The filtered value is level + trend (a one-step prediction) instead of the level. Default off.

The Holt filter is applied to each tracker sample, not to time, so the settings depend on the tracker sampling rate. Position and rotation (as a quaternion) are filtered separately. The first position is not changed. Setting alpha to 1 turns the smoothing off. The filter is done before the time calibration and the lag estimate.

Processed positions
-------------------

When a reconstruction is done, the positions that the reconstruction algorithm used are written next to the input data (the .fts file):

- `<name>.tp.corr`: One position (prMt, same format as the tracking position file .tp) for each frame that was used
- `<name>.tts.corr`: The time stamps (ms) of the positions, same format as the tracking timestamp file .tts

The positions are those after the position filter, the time calibration (including an estimated lag), and any correction done by the algorithm (e.g. the VIC image correlation correction). They are given at the frame times, so if the files are used as `.tp` and `.tts` they reproduce the same reconstruction when the Position Filter is off, no time calibration is used, and image correlation correction is off. Frames that were removed by the preprocessing are not included. When both a B-mode and an angio volume are reconstructed, the positions from the B-mode reconstruction are written.




\defgroup cx_user_doc_group_usreconstruction US Reconstruction Algorithms

All available US Reconstruction Algorithms

