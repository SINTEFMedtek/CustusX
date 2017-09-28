Build instructions {#build_instructions}
===================

Recommended build procedure {#build_instructions2}
===================

Make sure you:
* Are on a \ref supported_platforms (For building newer operating systems it might be nessecary to change to the develop branch).
* Fulfill the \ref prerequisites. 
* Have a github account with [SSH authentication](https://help.github.com/articles/set-up-git/).

Choose a root folder for the project. It will be populated as described in \ref build_instructions_folder_structure.

Then run the following commands:

	cd \<root_dir\>
	git clone git@github.com:SINTEFMedTek/CustusX.git CX/CX
	(git checkout develop)
	./CX/CX/install/cxInstaller.py --full --all -t Release

Run `cxInstaller.py -h` for more options.

Prerequisites {#prerequisites}
------------------------

 * [CMake](http://www.cmake.org/)
 * [Qt5](http://qt-project.org/) including the sqlite plugin.
 * [openCL](https://www.khronos.org/opencl/)
 * [boost](http://www.boost.org/)
 * [doxygen](http://www.doxygen.org/)
 * [cppunit](http://sourceforge.net/projects/cppunit/)
 * ... 

These must be installed prior to installing CustusX.

Several other libraries (see \ref external_libraries) are part of the \ref dev_superbuild,
and thus does not need to be installed separately. 

For convenience, setup scripts for some platforms are available in the 
repository. They will help setup a machine from scratch, but might give 
you more than you expected. Look for your platform in 
[install/platforms](https://github.com/SINTEFMedtek/CustusX/tree/master/install/platforms).

If you run into trouble, try building VTK, then CTK and run an example application, ctkPluginBrowser for instance. There is a wealth of resources on the net describing how to compile these projects. If this fails, try the specific resources on CustusX.

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

