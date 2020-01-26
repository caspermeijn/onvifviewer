#!/usr/bin/env python3

# Copyright (C) 2019 Casper Meijn <casper@meijn.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import xml.etree.ElementTree as ET
import argparse
from pathlib import Path
import os
import shutil

parser = argparse.ArgumentParser(description='Convert appstream metadata to fastlane metadata.')
parser.add_argument('--manifest', required=True, help='path to the Android manifest file')
parser.add_argument('--fastlane', required=True, help='path to the fastlane metadata directory')
args = parser.parse_args()

fastlane_path = Path(args.fastlane)
fastlane_path.mkdir(0o755, True, True)

tree = ET.parse(args.manifest)
root = tree.getroot()

version_code = root.attrib.get('{http://schemas.android.com/apk/res/android}versionCode')
if not version_code:
    sys.exit('version code not found in manifest')

changelog_dir = fastlane_path.joinpath('android/en-GB/changelogs/')
if not changelog_dir.exists():
    sys.exit('no changelog dir found in metadata directory')
    
current_changelog_path = changelog_dir.joinpath(version_code + ".txt")
    
best_changelog_path = ""
best_changelog_number = 0
for entry in os.scandir(str(changelog_dir.resolve())):
    if entry.is_file():
        entry_number = int(entry.name.split('.')[0])
        if entry_number > best_changelog_number:
            best_changelog_number = entry_number
            best_changelog_path = changelog_dir.joinpath(entry.name)


print("Copying", best_changelog_path, "to", current_changelog_path)
try:
    shutil.copyfile(str(best_changelog_path), str(current_changelog_path))
except (shutil.SameFileError):
    pass

