US Reconstruction Plugin {#org_custusx_usreconstruction}
===================

Overview {#org_custusx_usreconstruction_overview}
========================

Reconstruct 3D US data from acquired a 2D sequence.

\addindex us_reconstruction
US Reconstruction Widget {#org_custusx_usreconstruction_widgets_us_reconstruction}
===========================================================

Reconstruct 3D US data from acquired 2D sequence.

The widget has these tabs:

- **Reconstruct**: select the **2D US sequence** (a recording, .fts) and check what to create:
  - **3D US volume**: the B-mode volume
  - **Angio volume**: a volume of the color (Doppler) data. The recording must have color data.
  - **Angio mesh**: a mesh of the angio volume where it is above the *Angio mesh threshold*.
    Also creates the angio volume.
  - Outputs of plugins, such as segmentations.

  Press **Reconstruct** to create all of them, one after the other. **Cancel** stops after the
  current step. The created data are listed when the reconstruction is finished, each with an icon
  for its type (volume or mesh), a button that shows or hides it in the active view group, a color
  button for meshes and colored volumes, and a button that deletes it (press twice).
- **US settings**: the reconstruction settings and the size of the output volume.
- Tabs with the settings of plugins.

The angio settings of the reconstruction (Angio data, Dual Angio, and the US Angio or US B-Mode
preset) are set from the checked outputs. They are also used by the reconstruction after an
acquisition, and with 8 bit acquisition data, an acquisition only records color data when Angio
volume or Angio mesh is checked.

Select a US Reconstruction algorithm from the list in \ref cx_user_doc_group_usreconstruction.

Flip images L/R
---------------

Some recordings have images that are mirrored left to right relative to the tracking data, e.g. older acquisitions where the scanner image was flipped. With **Flip images L/R** on, the images are mirrored laterally before they are used, both in the reconstruction and in any image based correction done by the algorithm. The probe sector (origin and clip rect) is mirrored too, so the mask and the frame positions follow the images.

The option belongs to the selected recording: it is not saved, and it is turned off when another recording is selected. Some algorithms (e.g. VIC with image correlation correction) check the image orientation against the tracking data, and write a warning to the log if the images seem to be mirrored.

Position filter
---------------

The tracker positions can be filtered before they are used. The filter is selected with the *Position Filter* option:

- **Off**: The positions are used as they are.
- **Moving average** (default): Centered moving average over the tracker samples. The window is set by *Position Filter Strength* (0 = no filtering). The filter does not delay the data, but it reduces the amplitude of fast movements.
- **Holt**: Double exponential smoothing (Holt's linear method), which tracks both the level and the trend of the movement. The parameters are:
  - *Holt alpha* (0.01 to 1): Smoothing of the level. 1 gives no smoothing (when *Holt add trend* is off), a small value gives strong smoothing. Default 0.5.
  - *Holt beta* (0 to 1): Smoothing of the trend (the speed of the movement). Default 0.3.
  - *Holt both directions*: The filter is run both forward and backward in time and the mean of the two is used. This gives no delay of the positions. If turned off, the filter is causal and the filtered positions are delayed relative to the images, which acts as an added time lag. Default on.
  - *Holt add trend*: The filtered value is level + trend (a one-step prediction) instead of the level. Default off.

The Holt filter is applied to each tracker sample, not to time, so the settings depend on the tracker sampling rate. Position and rotation (as a quaternion) are filtered separately. The first position is not changed. The time calibration (also Align timestamps) and an estimated lag only change the time stamps, so they do not change the result of the filter.

Processed positions
-------------------

When **Save processed positions** is on, the positions that the reconstruction algorithm used are written next to the input data when a reconstruction is done. The files get the same name as the .fts file of the input data:

- `.tp.corr`: One position (prMt, same format as the tracking position file .tp) for each frame that was used
- `.tts.corr`: The time stamps (ms) of the positions, same format as the tracking timestamp file .tts

The positions are those after the position filter, the time calibration (including an estimated lag), and any correction done by the algorithm (e.g. an image correlation correction). They are given at the frame times, so if the files are used as `.tp` and `.tts` they reproduce the same reconstruction when the Position Filter is off, no time calibration is used, and image correlation correction is off. Frames that were removed by the preprocessing are not included. When both a B-mode and an angio volume are reconstructed, the positions from the B-mode reconstruction are written.




\defgroup cx_user_doc_group_usreconstruction US Reconstruction Algorithms

All available US Reconstruction Algorithms

