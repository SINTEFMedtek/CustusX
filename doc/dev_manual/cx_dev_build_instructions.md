Build instructions {#build_instructions}
===================

Recommended build procedure {#build_instructions2}
===================

Make sure you:
* Are on a \ref supported_platforms (For building on newer operating systems it might be nessecary to change to the develop branch).
* Fulfill the \ref prerequisites.
* Have a github account with [SSH authentication](https://help.github.com/articles/set-up-git/).

Choose a root folder for the project, e.g. `~/dev/cx`. It will be populated as described in \ref build_instructions_folder_structure.

Then run the following commands in your user's home folder:

    mkdir dev
    mkdir dev/cx
    cd dev/cx
    git clone git@github.com:SINTEFMedTek/CustusX.git CX/CX
    cd CX/CX
    (git checkout develop)
    ./install/cxInstaller.py --full --all --build_type Release --user_doc

Run `cxInstaller.py -h` for a list of the components in CustusX to build, and an explanation of the input arguments.
The `--full` argument is a combination of the following arguments:

 * `--checkout:` download the source code repositories of the selected components.
 * `--configure:` configure and run CMake to generate build files.
 * `--make:` build the selected components.

The `--all` argument selects all components for building. After having run the script successfully, you might want to
drop this argument and instead list the components you want to run commands on, primarily *CustusX*.

Prerequisites {#prerequisites}
------------------------

The following software must be installed prior to building *CustusX*.

Several other libraries (see \ref external_libraries) are part of the \ref dev_superbuild,
and thus does not need to be installed separately.

For convenience, setup scripts for some platforms are available in the
repository. They will help setup a machine from scratch, but might not be fully updated or give
you more than you expected. Look for your platform in
[install/platforms](https://github.com/SINTEFMedtek/CustusX/tree/master/install/platforms).

### Mac and Linux

On Linux, one can most likely use the package manager to install the required software below. On Mac, it is adviced to use
regular installers for:

 * **Xcode.** *Mac only.* Use the *App store* to download. You need to start *Xcode* after downloading it to finish the installation.
 * **CMake.** On Mac, add cmake to the PATH in your `.profile` file and restart the machine:<br>
`export PATH=/Applications/CMake.app/Contents/bin:$PATH`
 * **Qt open source.** We use Qt Creator as our IDE. On Mac you might need to add a path to the Qt CMake files to your `.profile` file
   (or .bashrc or .bash_profile) and restart the machine. Edit the path below so that it is correct on your machine:<br>
   `export CMAKE_PREFIX_PATH=/Users/"Your username"/"Qt installation path"/"Qt version number"/clang_64/lib/cmake`




The following software can be installed on Mac as well through a package manager like *MacPorts* or *HomeBrew*:

 * **Boost**
 * **Cppunit.** For the tests.
 * **Glew**
 * **Doxygen.** To build the user/developer documentation.
 * **[Ninja](https://ninja-build.org).** Increases build speed. The build script will detect that ninja is installed automatically.

**Python and Git**: Make sure that you have *Python* and *Git* installed and that you can run them from the command line.
To run the tests you need some additional python packages. You can use *pip* to install them:

    sudo easy_install pip
    sudo pip install lxml
    sudo pip install pycrypto

**Cuda**: On Linux you might need to install *Cuda* to get *OpenCL* working. It is
recommended to download and use the regular installer from the web site.

### Windows

The Windows build environment can be a bit harder to setup. After you have generated .ssh keys and
cloned the *CustusX* repo, you can look for the Windows setup scripts in the `CX/install/platforms` folder.
There is a readme file to follow. Basically, the most of the instructions are found there.
You must manually install *Visual studio* and then update the paths in the main script. The script will
download and install many applications, including the ones described above. It will also create a folder
in your build tree with a build environment to run the build script in, *CustusX_environment*.
You will use this command to run the build script:

    CX/install/run_python.bat win64 PATH_to_CustusX_environment "cxInstaller.py --full --all --build_type Release --user_doc"

After you have run the build script, you can use the `set_run_environment.bat` script in the build folder to start
*Qt Creator*. You also use this script to start the executable:

    CX/b_Release/set_run_environment.bat PATH_to_QtCreator
    set_run_environment.bat CustusX.exe

If you want to use *Qt Creator* for debugging, it is important to use the script in the *Debug* build folder to start it.

### Practical information

To start writing code, open the file

    root_dir/CX/CX/CMakeLists.txt

in *Qt Creator.* Make sure that the build folder(s) for your selected configuration(s) (Debug/Release)
matches the build folder(s) in your build tree.

On Mac/Linux, you have to add the path to ninja (most likely `/opt/local/bin`)
to the *PATH* variable in the *Build environment* in the prooject settings in Qt Creator.

You might need to rerun the build script, and CMake to get everything working.

## Running the tests

The test suite of *CustusX* can be run through the executable *Catch*, which is built as part of the
superbuild. Run it with the `-h` argument to see the options. To run a specific test:

    ./CX/build_Release/bin/Catch "test name"

Note that the test name must be in quotes. Tests are tagged. To run all the unit tests:

    ./CX/build_Release/bin/Catch [unit]~[hide]~[unstable]~[not_linux]

This will run all unit tests, but not tests tagged as hidden, unstable or not for Linux.
Run with the `-t` argument to list all the tags including the not-tags for other platforms, and `-l` to list all the tests.

The integration tests are generally more complex and should be run in separate processes.
We use a script for that:

    ./CX/install/cxRunTests.py --run_catch --wrap_in_ctest [integration]~[unstable]~[hide]~[not_linux]

Several tests uses data in the *CustusX-Data* component which is downloaded as part of the superbuild
as the *data* folder in the *CX/CX* folder.
Some tests might depend on external applications, like *Vlc*, being installed. Other tests might
depend on graphics drivers or other hardware. The complexity of the test suite means that you might
have failing tests, but it should generally not be a big concern if you are on one of the
listed \ref supported_platforms.

Some notes about running the tests on an installed (i.e. not built) version of *CustusX*:

 * At the time, some relative paths from the build machine excists in the *Catch* executable
   This means that you need to put the *data* repo in the corresponding location.
 * The script `cxRunTests.py` only works if you have installed *CustusX* in the same folder
   relative to the script, as if it was a built version. Also, the script doesn't work
   on a debug installation.

## Superbuild Folder Structure {#build_instructions_folder_structure}

The default CustusX folder structure differs from the standard CMake source+build
structure. All libraries, CustusX included, are placed within a root folder,
with source and build folders grouped according to library.

The CustusX \ref dev_superbuild defines and sets up this structure. It is fully
possible to use a different structure, in that case you must configure cmake
yourself.

|        |          |                |
| ------ | ----     | -------------- |
| root   | CX       | CX             |
|        |          | build_Release  |
|        | VTK      | VTK            |
|        |          | build_Release  |
|        | CTK      | CTK            |
|        |          | build_Release  |
|        | some_lib | some_lib       |
|        |          | build_Release  |
