#!/usr/bin/env python3

#####################################################
# Print a short hash identifying one external library component's own
# pinned version/config, for per-library CI package-registry cache keys
# (CustusX#50).
#
# Unlike hashing the whole cxComponents.py file (the previous scheme), this
# only changes when the resolved class -- or one of its base classes, e.g. a
# shared CppComponent helper -- actually changes, so an edit to one
# library's pinned version doesn't invalidate every other library's cached
# package.
#
# Resolves which class a given library name maps to the same way a real
# build does, via cxComponentAssembly.LibraryAssembly (respecting --igstk),
# so e.g. "VTK" correctly hashes the VTK92 class under --igstk and the VTK
# class otherwise -- matching cxInstaller.py's own component selection
# (cxComponentAssembly.selectLibraries(), matched by component.name()).
#
# Usage: python3 cxLibVersionHash.py <library-name> [--igstk]
#####################################################

import argparse
import contextlib
import hashlib
import inspect
import io
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from cx.build import cxComponentAssembly, cxInstallData


def hashForComponent(component):
    '''
    Hash the component's own class source plus every base class up to (not
    including) object, via the MRO -- so a change to shared build machinery
    (e.g. CppComponent.configureCMake()) invalidates every library's cache,
    not just the one class actually edited, while an edit to one library's
    own class (e.g. bumping VTK's pinned tag) only invalidates that one.
    '''
    parts = []
    for klass in type(component).__mro__:
        if klass is object:
            continue
        try:
            parts.append(inspect.getsource(klass))
        except (OSError, TypeError):
            # Dynamically-created or built-in classes have no retrievable
            # source; nothing in this codebase's component hierarchy should
            # hit this, but don't crash the cache-key computation over it.
            pass
    combined = '\n'.join(parts)
    return hashlib.sha256(combined.encode('utf-8')).hexdigest()[:16]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('library_name', help='Library name as used by cxInstaller.py\'s component selection (e.g. VTK, eigen)')
    parser.add_argument('--igstk', action='store_true', help='Resolve as the --igstk build variant')
    args = parser.parse_args()

    # cxInstallData.Common() calls cxRepoHandler.getBranchForRepo(), which
    # unconditionally prints the git command it runs and its output via
    # runShell() -- silence that here so this script's stdout is exactly the
    # hash and nothing else. A caller doing HASH=$(python3
    # cxLibVersionHash.py ...) would otherwise capture that noise into
    # $HASH too, producing a multi-line "hash" that breaks a URL built from
    # it (observed in CI: curl exit code 3, "malformed URL").
    with contextlib.redirect_stdout(io.StringIO()):
        data = cxInstallData.Common()
        data.mBuildIGSTK = args.igstk
        assembly = cxComponentAssembly.LibraryAssembly(controlData=data)

    for component in assembly.libraries:
        if component.name() == args.library_name:
            print(hashForComponent(component))
            return

    print('ERROR: no component named %r found (igstk=%s)' % (args.library_name, args.igstk), file=sys.stderr)
    sys.exit(1)


if __name__ == '__main__':
    main()
