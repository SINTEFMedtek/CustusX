#!/usr/bin/env python3

#####################################################
# Upload a locally-built Windows installer to GitLab and add it to the
# release page.  Run this after building the installer locally (no Windows
# CI runner) and after the CI pipeline has already created the release.
#
# Usage (dry-run — shows what would happen):
#   python install/cxUploadWindowsInstaller.py --version v26.08-rc5 --app fraxinus --installer C:/path/to/Fraxinus-win64.exe
#
# Usage (execute):
#   python install/cxUploadWindowsInstaller.py --version v26.08-rc5 --app fraxinus --installer C:/path/to/Fraxinus-win64.exe --execute
#
# Requires a GitLab personal access token with the 'api' scope stored in the
# GITLAB_TOKEN environment variable.  Create one at:
#   https://gitlab.sintef.no/-/user_settings/personal_access_tokens
#
# Store it in your shell:
#   Linux/Mac:  export GITLAB_TOKEN=your_token   (add to ~/.bashrc)
#   Windows PS: $env:GITLAB_TOKEN = "your_token"
#   Windows cmd (permanent): setx GITLAB_TOKEN your_token
#
# Prerequisites:
#   - The GitLab CI pipeline for the tag must have already run and created
#     the release (this script adds a link to an existing release).
#   - curl must be available on PATH (included in Windows 10 1803+ and all
#     modern Linux/Mac systems).
#####################################################

from __future__ import print_function
import argparse
import json
import os
import subprocess
import sys

GITLAB_BASE = 'https://gitlab.sintef.no'

APP_CONFIG = {
    'custusx': {
        'project_path': 'custusx/custusx',
        'package_name': 'CustusX',
        'link_name': 'CustusX Windows installer',
        'upload_filename': 'CustusXWindows.exe',
    },
    'custusx-s': {
        'project_path': 'custusx/custuss',
        'package_name': 'CustusS',
        'link_name': 'CustusS Windows installer',
        'upload_filename': 'CustusSWindows.exe',
    },
    'fraxinus': {
        'project_path': 'custusx/fraxinus',
        'package_name': 'Fraxinus',
        'link_name': 'Fraxinus Windows installer',
        'upload_filename': 'FraxinusWindows.exe',
    },
    'fraxinus-private': {
        'project_path': 'custusx/org.custusx.fraxinus.private',
        'package_name': 'Fraxinus',
        'link_name': 'Fraxinus Private Windows installer',
        'upload_filename': 'FraxinusPrivateWindows.exe',
    },
}


def _api_url(project_path, endpoint):
    encoded_project = project_path.replace('/', '%2F')
    return '%s/api/v4/projects/%s/%s' % (GITLAB_BASE, encoded_project, endpoint)


def _run(description, cmd, dry_run):
    print('\n--- %s ---' % description)
    # Mask the token value in displayed output
    display_cmd = []
    skip_next = False
    for part in cmd:
        if skip_next:
            display_cmd.append('***')
            skip_next = False
        elif part == 'PRIVATE-TOKEN:':
            display_cmd.append(part)
            skip_next = True
        else:
            display_cmd.append(part)
    print('  $ ' + ' '.join(display_cmd))
    if not dry_run:
        result = subprocess.run(cmd, check=False)
        if result.returncode != 0:
            print('\nERROR: command failed (exit code %d)' % result.returncode)
            sys.exit(result.returncode)


def main():
    parser = argparse.ArgumentParser(
        description='Upload a Windows installer to GitLab and add it to the release page.',
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument('--version', required=True,
                        help='Release tag, e.g. v26.08-rc5 or v26.08')
    parser.add_argument('--app', required=True, choices=sorted(APP_CONFIG),
                        help='Application: %s' % ' | '.join(sorted(APP_CONFIG)))
    parser.add_argument('--installer', required=True,
                        help='Path to the .exe installer file')
    parser.add_argument('--execute', action='store_true',
                        help='Actually run. Default is dry-run (print commands only).')
    args = parser.parse_args()

    dry_run = not args.execute
    print('=== %s: %s  app=%s ===' % ('DRY RUN' if dry_run else 'EXECUTING', args.version, args.app))
    if dry_run:
        print('(add --execute to actually run)\n')

    if not os.path.isfile(args.installer):
        print('ERROR: installer not found: %s' % args.installer)
        sys.exit(1)

    token = os.environ.get('GITLAB_TOKEN', '')
    if not token and not dry_run:
        print('ERROR: GITLAB_TOKEN environment variable not set.')
        sys.exit(1)
    display_token = token if token else '<GITLAB_TOKEN>'

    cfg = APP_CONFIG[args.app]
    project_path = cfg['project_path']

    package_url = _api_url(
        project_path,
        'packages/generic/%s/%s/%s' % (cfg['package_name'], args.version, cfg['upload_filename']),
    )

    _run(
        'Upload installer to GitLab Generic Packages',
        [
            'curl', '--fail', '--progress-bar',
            '--header', 'PRIVATE-TOKEN: %s' % display_token,
            '--upload-file', args.installer,
            package_url,
        ],
        dry_run,
    )

    link_data = json.dumps({
        'name': cfg['link_name'],
        'url': package_url,
        'link_type': 'package',
    })
    releases_links_url = _api_url(project_path, 'releases/%s/assets/links' % args.version)

    _run(
        'Add link to GitLab release page',
        [
            'curl', '--fail', '--request', 'POST',
            '--header', 'PRIVATE-TOKEN: %s' % display_token,
            '--header', 'Content-Type: application/json',
            '--data', link_data,
            releases_links_url,
        ],
        dry_run,
    )

    print('\nDone. Check the release page at:')
    print('  %s/%s/-/releases/%s' % (GITLAB_BASE, project_path, args.version))


if __name__ == '__main__':
    main()
