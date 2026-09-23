#!/usr/bin/env python3

"""
Diff a consumer repo's vendored copy of a CX/CX file against CX/CX's own
canonical copy, and report any drift.

Some files (cxRepoHandler.py chief among them) are vendored as physical
copies into CS/CS and FX/FX rather than imported, because they are
exactly the bootstrap code that has to run *before* CX is checked out,
in order to check out CX in the first place - an import can't solve that
chicken-and-egg problem. That's a structural necessity, not a design
flaw - but a vendored copy can silently drift once nobody remembers to
re-copy a fix made to the canonical one. This makes that drift visible
instead of silent: cxRepoHandler.py's cloneRepoWithPrompt() in CustusS's
own copy deleted an existing directory *before* prompting the user
instead of after (CX/CX's own copy always prompted first) - a real,
safety-relevant behavior difference that had no reason to be caught
before a script like this exists.

Usage:
    python3 cxCheckVendoredFileSync.py <canonical_path>=<vendored_path> [...]

Each argument is a pair of paths (absolute, or relative to the caller's
cwd) to compare byte-for-byte. Exit code is the number of pairs that
differ or could not be compared (0 = everything in sync), so a CI job
can wire this in with an allow_failure exit-code list (this project's
usual CustusX#46/#50 convention for "warn, don't hard-fail the pipeline")
instead of a hard pass/fail.
"""
from __future__ import print_function
import difflib
import os
import sys


def main(argv):
    if len(argv) < 1:
        print('Usage: cxCheckVendoredFileSync.py <canonical_path>=<vendored_path> [...]', file=sys.stderr)
        return 2

    mismatches = 0
    for pair in argv:
        if '=' not in pair:
            print('ERROR: malformed pair (expected canonical_path=vendored_path): %s' % pair, file=sys.stderr)
            mismatches += 1
            continue
        canonical_path, vendored_path = pair.split('=', 1)

        if not os.path.isfile(canonical_path):
            print('ERROR: canonical file not found: %s' % canonical_path, file=sys.stderr)
            mismatches += 1
            continue
        if not os.path.isfile(vendored_path):
            print('ERROR: vendored file not found: %s' % vendored_path, file=sys.stderr)
            mismatches += 1
            continue

        with open(canonical_path) as f:
            canonical_lines = f.readlines()
        with open(vendored_path) as f:
            vendored_lines = f.readlines()

        if canonical_lines == vendored_lines:
            print('  in sync: %s' % vendored_path)
            continue

        mismatches += 1
        print('\nDRIFTED: %s no longer matches %s' % (vendored_path, canonical_path))
        diff = difflib.unified_diff(canonical_lines, vendored_lines,
                                     fromfile=canonical_path, tofile=vendored_path)
        sys.stdout.writelines(diff)

    print('\n%d of %d vendored file(s) drifted from their canonical CX/CX copy.' % (mismatches, len(argv)))
    return mismatches


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
