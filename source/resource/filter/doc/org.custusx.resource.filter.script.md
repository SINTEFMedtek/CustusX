Script Filter {#org_custusx_resource_filter_script}
===========================================================

\addindex generic_script_filter

Support for calling external scripts from CustusX.

Uses a .ini file for configuration:

script:
- path: relative/abolute path to script file to run
- arguments: additional input arguments to the script if required

environment:
- path: path to an environment (program) for running the script. E.g.: /opt/local/bin/python or ./scripts/python_example/venv/bin/python. May be empty if the script is executeable

output:
- file_append: the output file will have same name as the input file with this text appended


On "run", CustusX generates a command-line call using the environment path as first element, followed by the script path, an abolute path to the selected volume and then an abolute path to the expected output file. The arguments line follows as the last elements of the command.
When the script has finished, CustusX looks for the expected output file and includes this in the data for the selected patient if it exist.

NB: This plugin is under development, usage may change in future versions.

\addtogroup cx_user_doc_group_filter

* \ref org_custusx_resource_filter_script
