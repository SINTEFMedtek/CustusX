#!/bin/bash

#  linux_aliases.sh
#
#  Useful aliases for running tests
#  To apply aliases to current shell. Run script this way: . ./linux_aliases.sh
#
#  Created by Ole Vegard Solberg on 1/30/14.
#

alias appleunitw="~/dev/working/CustusX3/CustusX3/install/Shared/script/cxRunTests.py --run_catch --wrap_in_ctest [unit]~[unstable]~[hide]~[not_apple]"
alias appleunit="~/dev/working/CustusX3/CustusX3/install/Shared/script/cxRunTests.py --run_catch [unit]~[unstable]~[hide]~[not_apple]"
alias appleint="~/dev/working/CustusX3/CustusX3/install/Shared/script/cxRunTests.py --run_catch --wrap_in_ctest [integration]~[unstable]~[hide]~[not_apple]"


alias unitw="~/dev/working/CustusX3/CustusX3/install/Shared/script/cxRunTests.py --run_catch --wrap_in_ctest [unit]~[unstable]~[hide]~[not_linux]"
alias unit="~/dev/working/CustusX3/CustusX3/install/Shared/script/cxRunTests.py --run_catch [unit]~[unstable]~[hide]~[not_linux]"
alias int="~/dev/working/CustusX3/CustusX3/install/Shared/script/cxRunTests.py --run_catch --wrap_in_ctest [integration]~[unstable]~[hide]~[not_linux]"
