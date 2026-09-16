#!/usr/bin/env python3

#####################################################
# Upload a locally-built installer (Windows .exe or Mac .dmg) to GitLab and
# add it to the release page. Run this after building the installer locally
# (no Windows/Mac CI runner) and after the CI pipeline has already created
# the release. Which OS it's for is detected from the installer file's own
# extension -- nothing else to specify.
#
# Usage (dry-run — shows what would happen):
#   python install/cxUploadInstaller.py --version v26.08-rc5 --app Fraxinus --installer C:/path/to/Fraxinus-win64.exe
#   python install/cxUploadInstaller.py --version v26.08-rc5 --app Fraxinus --installer /path/to/Fraxinus.dmg
#
# Usage (execute):
#   python install/cxUploadInstaller.py --version v26.08-rc5 --app Fraxinus --installer C:/path/to/Fraxinus-win64.exe --execute
#
# Requires a GitLab personal access token with the 'api' scope.  Create one at:
#   https://gitlab.sintef.no/-/user_settings/personal_access_tokens
#
# Pass the token via --token, or store it so you don't have to type it each time:
#   Linux/Mac:  export GITLAB_TOKEN=your_token   (add to ~/.bashrc to persist)
#   Windows PS: $env:GITLAB_TOKEN = "your_token"              (current session)
#               [System.Environment]::SetEnvironmentVariable("GITLAB_TOKEN","your_token","User")
#                                                             (permanent, all sessions)
#   Windows cmd: set GITLAB_TOKEN=your_token                  (current session)
#                setx GITLAB_TOKEN your_token                 (permanent, all sessions)
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
    'CustusX': {
        'project_path': 'custusx/custusx',
        'package_name': 'CustusX',
    },
    'CustusS': {
        'project_path': 'custusx/custuss',
        'package_name': 'CustusS',
    },
    'Fraxinus': {
        'project_path': 'custusx/fraxinus',
        'package_name': 'Fraxinus',
    },
    'FraxinusExcelsior': {
        'project_path': 'custusx/org.custusx.fraxinus.private',
        'package_name': 'FraxinusExcelsior',
    },
}

# Keyed by the installer file's own extension -- this is how the OS an
# installer is for gets determined, no separate --os flag needed.
OS_LABEL_BY_EXTENSION = {
    '.exe': 'Windows',
    '.dmg': 'Mac',
}


def _os_label_for_installer(installer_path):
    ext = os.path.splitext(installer_path)[1].lower()
    label = OS_LABEL_BY_EXTENSION.get(ext)
    if not label:
        print('ERROR: unrecognized installer file extension %r (expected one of: %s)' %
              (ext, ', '.join(sorted(OS_LABEL_BY_EXTENSION))))
        sys.exit(1)
    return label, ext


def _api_url(project_path, endpoint):
    encoded_project = project_path.replace('/', '%2F')
    return '%s/api/v4/projects/%s/%s' % (GITLAB_BASE, encoded_project, endpoint)


def _run(description, cmd, dry_run, secret=None):
    print('\n--- %s ---' % description)
    # Mask the token value in displayed output, wherever it appears in an arg
    # (e.g. embedded in "PRIVATE-TOKEN: <token>") rather than assuming argv shape.
    display_cmd = cmd
    if secret:
        display_cmd = [part.replace(secret, '***') for part in cmd]
    print('  $ ' + ' '.join(display_cmd))
    if not dry_run:
        result = subprocess.run(cmd, check=False)
        if result.returncode != 0:
            print('\nERROR: command failed (exit code %d)' % result.returncode)
            sys.exit(result.returncode)


def main():
    parser = argparse.ArgumentParser(
        description='Upload a Windows (.exe) or Mac (.dmg) installer to GitLab and add it to the release page.',
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument('--version', required=True,
                        help='Release tag, e.g. v26.08-rc5 or v26.08')
    parser.add_argument('--app', required=True, choices=sorted(APP_CONFIG),
                        help='Application: %s' % ' | '.join(sorted(APP_CONFIG)))
    parser.add_argument('--installer', required=True,
                        help='Path to the installer file (.exe or .dmg -- determines which OS this upload is for)')
    parser.add_argument('--token',
                        help='GitLab personal access token (api scope). '
                             'Alternatively set the GITLAB_TOKEN environment variable.')
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

    os_label, ext = _os_label_for_installer(args.installer)

    token = args.token or os.environ.get('GITLAB_TOKEN', '')
    if not token and not dry_run:
        print('ERROR: no GitLab token found. Pass --token <token> or set the GITLAB_TOKEN environment variable.')
        sys.exit(1)
    header_token = token if token else '<GITLAB_TOKEN>'

    cfg = APP_CONFIG[args.app]
    project_path = cfg['project_path']
    upload_filename = '%s%s%s' % (cfg['package_name'], os_label, ext)

    package_url = _api_url(
        project_path,
        'packages/generic/%s/%s/%s' % (cfg['package_name'], args.version, upload_filename),
    )

    _run(
        'Upload installer to GitLab Generic Packages',
        [
            'curl', '--fail', '--progress-bar',
            '--header', 'PRIVATE-TOKEN: %s' % header_token,
            '--upload-file', args.installer,
            package_url,
        ],
        dry_run,
        secret=token,
    )

    link_data = json.dumps({
        'name': '%s %s installer' % (cfg['package_name'], os_label),
        'url': package_url,
        'link_type': 'package',
    })
    releases_links_url = _api_url(project_path, 'releases/%s/assets/links' % args.version)

    _run(
        'Add link to GitLab release page',
        [
            'curl', '--fail', '--request', 'POST',
            '--header', 'PRIVATE-TOKEN: %s' % header_token,
            '--header', 'Content-Type: application/json',
            '--data', link_data,
            releases_links_url,
        ],
        dry_run,
        secret=token,
    )

    print('\nDone. Check the release page at:')
    print('  %s/%s/-/releases/%s' % (GITLAB_BASE, project_path, args.version))


if __name__ == '__main__':
    main()
