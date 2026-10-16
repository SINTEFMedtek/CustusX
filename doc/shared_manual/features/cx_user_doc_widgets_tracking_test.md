Tracking Test Widget {#tracking_test_widget}
===========================================================

\addindex tracking_test_widget

The Tracking Test widget is used for testing the tracking hardware and the tracking systems in
CustusX: NDI (Aurora and Polaris), IGSTK, and OpenIGTLink. Open it from `Window->Debugging->Tracking Test`.
It has three tabs:

* Record positions: record the tool positions to a file while you use the tools.
* Compare systems: track the same tools with several tracking systems, one after the other, and
  compare the positions.
* Tracking state: start and stop the tracking one step at a time.

The tool buttons in the status bar show the state of each tool while you test:

* Gray: the tool is configured, but not connected to the tracking hardware.
* Red: the tool is not visible.
* Green: the tool is visible. Light green is the active tool.
* Orange: the tool is visible, but the position may be inaccurate. Hover over the button to see why,
  e.g. when an NDI tool is (partially) out of the measurement volume, the error is high, or a marker
  sphere on a Polaris tool is hidden. Light orange is the active tool.

Record positions {#tracking_test_widget_record}
===========================================================

\addindex tool_position_recorder_widget

Records the positions of all the tracked tools to a CSV file, which can be opened in a spreadsheet
or analysed in e.g. Python.

1. Start tracking (e.g. `Tracking->Start Tracking`).
2. Select a file, or use the suggested file name in your home folder.
3. Enter a label, e.g. the tracking system or what you are testing. It is written in the first column.
4. Press Start recording, use the tools, and press Stop recording.

While recording, the widget shows the number of rows, and for each tool the number of positions,
the rate, the number of positions with a warning, the mean position, the jitter (standard deviation)
and the drift (the change in mean position from the first to the last second).

Some things to test this way:

* Keep a tool still, to measure the jitter and drift.
* Move a tool in and out of the measurement volume, and to and from its border.
* Polaris: hide one of the spheres, partly or completely.
* Hide the reference: all the tools should become invisible.

The file has one row for each position, visibility change and position warning, with the columns:

* label, tool: the label above, and the tool uid.
* event: position, visible, hidden or warning.
* timestamp_ms: the time of the position, from the tracking system.
* received_ms: when CustusX received it.
* visible: 1 or 0.
* x, y, z: the tool tip in the patient reference space (mm), r00 to r22: the rotation.
* warning: why the position may be inaccurate, empty if it is OK.
* metadata: extra information from the tracking system. For NDI: the RMS error, the port status and
  the number of markers used.

Compare systems {#tracking_test_widget_compare}
===========================================================

\addindex tracking_comparison_widget

Tracks the same tools with several tracking systems in turn, records the positions for a while with
each of them, and compares the results. Only one system can use the tracking device at a time, so the
systems are run one after the other: tracking is stopped, and the OpenIGTLink server closed, before
the next system starts.

1. Connect the tracking device and the tools, and place the tools so that they are visible. Keep them
   still during the comparison if you want to compare the positions.
2. Select the tool configuration:
   * The current tool configuration (from the Preferences) is selected by default.
   * The list also has the other tool configurations of the profile, and the test configurations of
     the Laboratory profile: LabDefault.xml (Polaris NDI reference frame and pointer) and
     LabAuroraTools.xml (Aurora reference on port 1, pointer on port 2, and sensors on ports 3 and 4).
   * Browse to use any other tool configuration file.
   The tracking system in the configuration doesn't matter, it is replaced for each system.
3. Select the tracking systems to compare: igstk, ndi and/or openigtlink. Only the systems that are
   built into this CustusX are available: usually either IGSTK or NDI.
4. For openigtlink, either start an OpenIGTLink tracking server yourself, or enter the server
   application and its arguments to have it started and stopped for you, for example:
   * OpenIGTLink server: /path/to/openigtlink_tracking_server
   * Server arguments: --hostname=/dev/ttyUSB0 --tools=ref.rom,pointer.rom
   The server is started in its own folder, so that it finds configuration files next to it. A leading
   ~ in the server path is replaced with the home folder.
   When the server is stopped, it first gets a newline on its standard input, then a terminate signal,
   and finally it is killed.
   The OpenIGTLink tools are found by the OpenIGTLink transform id in each tool file, which must match
   the device names the server sends. Keep "Apply the reference to the OpenIGTLink tools" on for servers
   that send the positions in tracker space, so that the positions are relative to the
   reference as for the other systems. Turn it off if the server applies the reference itself.
5. Select how long to record with each system, and the output folder.
6. Press Start comparison. The progress is shown below. Press Cancel to stop. When it is done, a table
   shows each tool (except the reference) for each system that worked: the tracking rate, the number of
   positions and of positions with a warning, the jitter (length of the standard deviation of the
   positions), the drift, and the distance between its mean position and the first system's.

For each system, the comparison copies the tool configuration with that tracking system, starts the
server (if any), starts tracking, waits for the tools to settle, records the positions, stops tracking
and the server, and waits a few seconds for the device to be released.

The results are in a new folder in the output folder, named tracking_comparison with the date and time:

* N_system_positions.csv: the positions recorded with each system, in the same format as above.
* N_system_toolconfig.xml: the tool configuration used.
* N_system_command.log: the output of the OpenIGTLink server.
* summary.txt: OK or FAILED for each system, the statistics of each tool, and how far the mean position
  of each tool is from its mean position with the first system. With still tools, the differences should
  be well below a millimeter.

Tracking state {#tracking_test_widget_state}
===========================================================

\addindex tool_manager_widget

Configure, initialize and start tracking, and the reverse, one step at a time. Useful for finding out
which step fails, e.g. connecting to the device (initialize) or starting the tracking.
