#!/bin/python3
# This script will analyze the language .ini files, and identify any missing keys for each file

import os, sys

ini_dir = "resin/res/i18n/"

# for each .ini file, track the keys
keys = {} # keys[filename] = [key1, key2, ...]

all_keys = set()

for filename in os.listdir(ini_dir):
    if filename.endswith(".ini"):
        # remove .ini ending
        with open(ini_dir + filename, 'r') as f:
            filename = filename[:-4]
            keys[filename] = []
            for line in f:
                if line.startswith(":"):
                    continue
                if "=" in line:
                    key = line.split("=")[0].strip()
                    keys[filename].append(key)
                    all_keys.add(key)

# for each .ini file, compare the keys to the keys in the other files
noneMissing = True
for filename in keys:
    cur_keys = set(keys[filename])
    for key in all_keys:
        if key not in cur_keys:
            noneMissing = False
            print(f"[{filename}] Missing key: {key}")

if noneMissing:
    print("All keys present! 🎉")