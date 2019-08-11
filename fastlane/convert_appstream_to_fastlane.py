#!/usr/bin/env python3

# Copyright (C) 2019 Casper Meijn <casper@meijn.net>
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

parser = argparse.ArgumentParser(description='Convert appstream metadata to fastlane metadata.')
parser.add_argument('--appstream', required=True, help='path to the appstream metadata file')
parser.add_argument('--fastlane', required=True, help='path to the fastlane metadata directory')
args = parser.parse_args()

fastlane_path = Path(args.fastlane)
fastlane_path.mkdir(0o755, True, True)

def get_lang_from_attib(tag):
    lang = tag.attrib.get('{http://www.w3.org/XML/1998/namespace}lang', 'en-GB')
    lang = lang.replace('_', '-')
    if lang == 'nb-NO':
        lang = 'no-NO'
    if lang == 'de':
        lang = 'de-DE'
    if lang == 'it':
        lang = 'it-IT'
    return lang

tree = ET.parse(args.appstream)
root = tree.getroot()

for name in root.findall('name'):
    lang = get_lang_from_attib(name)
    lang_dir = fastlane_path.joinpath(lang)
    lang_dir.mkdir(0o755, True, True)
    name_file = lang_dir.joinpath('title.txt')
    name_file.write_text(name.text)

for summary in root.findall('summary'):
    lang = get_lang_from_attib(summary)
    lang_dir = fastlane_path.joinpath(lang)
    lang_dir.mkdir(0o755, True, True)
    desc_file = lang_dir.joinpath('short_description.txt')
    desc_file.write_text(summary.text)

description_map = {}
for description in root.findall('./description/p'):
    lang = get_lang_from_attib(description)
    if lang in description_map:
        description_map[lang] = description_map[lang] + '\n'
    else:
        description_map[lang] = ""
    description_map[lang] = description_map[lang] + description.text + '\n'

for lang, text in description_map.items():
    lang_dir = fastlane_path.joinpath(lang)
    lang_dir.mkdir(0o755, True, True)
    desc_file = lang_dir.joinpath('full_description.txt')
    desc_file.write_text(text)

lang = 'en-GB'
for release in root.findall('./releases/release'):
    version = release.attrib.get('version')
    base_versioncode = 0
    for version_part in version.split('.'):
        base_versioncode = int(version_part) + base_versioncode * 100
    base_versioncode *= 100
        
    text = ""
    for paragraph in release.findall('./description/*'):
        if paragraph.tag == 'p':
            paragraph_lang = get_lang_from_attib(paragraph)
            if paragraph_lang == lang:
                text += paragraph.text + '\n'
        else:
            for listitem in paragraph.findall('li'):
                paragraph_lang = get_lang_from_attib(listitem)
                if paragraph_lang == lang:
                    text += '- ' + listitem.text + '\n'

    lang_dir = fastlane_path.joinpath(lang)
    changelog_dir = lang_dir.joinpath('changelogs')
    changelog_dir.mkdir(0o755, True, True)
    
    # From version 0.12, multiple APKs are made; 0 = base APK, 1 = armv7, 2 = aarch64
    if base_versioncode >= 1200:
        versioncode = base_versioncode * 10
        versioncodelist = [versioncode, versioncode + 1, versioncode + 2]   
    else:
        versioncodelist = [base_versioncode]  
    for versioncode in versioncodelist:
        changelog_file = changelog_dir.joinpath(str(versioncode) + '.txt')
        changelog_file.write_text(text)

fastlane_path.touch()
