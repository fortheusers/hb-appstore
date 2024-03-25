#!/bin/python3
import os

# for every file in the current directory
for filename in os.listdir('.'):
    # if it's an svg
    if filename.endswith('.svg') and not filename.endswith('_light.svg') and not filename.endswith('_gray.svg'):
        # open the file
        with open(filename, 'r') as file:
            # read the file
            data = file.read()
            # replace the fill color
            # data = data.replace('#FFFFFF', '#000000')
            data = data.replace('#FFFFFF', '#555555')
        # write the file
        # with open(filename.replace('.svg', '_light.svg'), 'w') as file:
        with open(filename.replace('.svg', '_gray.svg'), 'w') as file:
            file.write(data)

print("Done!")