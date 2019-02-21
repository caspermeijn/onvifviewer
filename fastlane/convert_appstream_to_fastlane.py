#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import argparse
from pathlib import Path

parser = argparse.ArgumentParser(description='Convert appstream metadata to fastlane metadata.')
parser.add_argument('--appstream', required=True, help='path to the appstream metadata file')
parser.add_argument('--fastlane', required=True, help='path to the fastlane metadata directory')
args = parser.parse_args()

fastlane_path = Path(args.fastlane)
if not fastlane_path.exists():
    sys.exit("fastlane path doesn't exist")

def get_lang_from_attib(tag):
    lang = tag.attrib.get('{http://www.w3.org/XML/1998/namespace}lang', 'en-GB')
    if lang == 'nb-NO':
        lang = 'no-NO'
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
    text += '\nNOTE: The video codec of Android doesn\'t support all types of RTSP streams. Therefore currently the video is not a live stream, but a series of snapshots.'
    desc_file.write_text(text)

lang = 'en-GB'
for release in root.findall('./releases/release'):
    version = release.attrib.get('version')
    versioncode = 0
    for version_part in version.split('.'):
        versioncode = int(version_part) + versioncode * 100
    versioncode *= 100

    text = ""
    for paragraph in release.findall('./description/*'):
        if paragraph.tag == 'p':
            text += paragraph.text + '\n'
        else:
            for listitem in paragraph.findall('li'):
                text += '- ' + listitem.text + '\n'

    lang_dir = fastlane_path.joinpath(lang)
    changelog_dir = lang_dir.joinpath('changelogs')
    changelog_dir.mkdir(0o755, True, True)
    changelog_file = changelog_dir.joinpath(str(versioncode) + '.txt')
    changelog_file.write_text(text)

fastlane_path.touch()
