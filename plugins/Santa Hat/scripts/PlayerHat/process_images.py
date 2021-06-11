import pickle
import json
import os

import math
from PIL import Image

IMG_PATH = 'img/'

with open('char-spr-data', 'rb') as f:
	char_data = pickle.load(f)

RED = (255, 0, 0, 255)
GREEN = (0, 255, 0, 255)

out_anims = dict()
process_chars = ['dustman', 'dustgirl', 'dustworth', 'dustkid']

for char_name in char_data:
	if char_name not in process_chars: continue

	anims_data = char_data[char_name]
	print('====== Processing char ' + char_name)
	sprite_path = '%s/%s/' % (IMG_PATH, char_name)

	for anim_name, frames in anims_data.items():
		print('   [' + anim_name + ']')
		frame_index = 1
		anim_path = '%s/%s-1-' % (sprite_path, anim_name)
		anim_data = anims_data[anim_name]
		prev_x, prev_y, prev_angle = 0, 0, 0

		processed_frames = 0
		out_frame_string = ''
		missing_data = []
		for frame in frames:
			frame_path = '%s%04d.png' % (anim_path, frame_index)

			start = None
			end = None

			if os.path.exists(frame_path):
				# print('      ' + str(frame) + '  ' + frame_path)
				img = Image.open(frame_path)
				pixdata = img.load()

				must_break = False

				for x in range(0, img.size[0]):
					for y in range(0, img.size[1]):
						if pixdata[x, y] == RED:
							start = (x, y)
							if end is not None:
								must_break = True
								break
						elif pixdata[x, y] == GREEN:
							end = (x, y)
							if start is not None:
								must_break = True
								break
					if must_break: break

				if start is None or end is None:
					missing_data.append('Missing data for ' + frame_path)

			if start is not None and end is not None:
				dx = end[0] - start[0]
				dy = end[1] - start[1]
				mid_x = start[0] + dx * 0.5 + frame['x']
				mid_y = start[1] + dy * 0.5 + frame['y']
				if out_frame_string != '': out_frame_string += ','
				out_frame_string += '%.1f,%.1f,%.4f' % (mid_x, mid_y, math.atan2(dy, dx))
				# print('      %.1f %.1f %.4f' % (mid_x, mid_y, math.atan2(-dx, dy)))

				processed_frames += 1

			frame_index += 1
			# break

		if processed_frames > 0:
			for msg in missing_data:
				print(msg)

		if anim_name in out_anims:
			out_anim_data = out_anims[anim_name]
		else:
			out_anim_data = out_anims[anim_name] = dict()

		out_anim_data[char_name] = out_frame_string
		pass

anim_index = 0
out_anim_index = []
out_char_frames = dict()

for anim_name, anim_data_chars in out_anims.items():
	out_anim_index.append('{\'%s\',%d}' % (anim_name, anim_index))

	for char_name, frames in anim_data_chars.items():
		if char_name not in out_char_frames:
			out_frames = out_char_frames[char_name] = []
		else:
			out_frames = out_char_frames[char_name]
		out_frames.append('/*%d*/ {%s}' % (anim_index, frames))

	anim_index += 1

out_char_map = []
out_char_items = []
out_final = ''
out_final += 'const dictionary ANIM_INDEX = {%s};\n' % (','.join(out_anim_index))
char_index = 0
for char_name, char_frames in out_char_frames.items():
	out_char_map.append('{\'%s\', %d}' % (char_name, char_index))
	out_char_items.append('{\n%s\n}\n' % ',\n'.join(char_frames))
	char_index += 1
out_final += 'const array<array<array<float>>> HAT_DATA = {\n%s};\n' % ',\n'.join(out_char_items)
out_final += 'const dictionary CHAR_INDICES = {%s};\n' % ',\n'.join(out_char_map)

# out_dir = 'C:/Users/Carl/AppData/Roaming/Dustforce/user/script_src/mod_sh/'
out_dir = ''
with open(out_dir + 'hat_data.cpp', 'w') as f:
	f.write(out_final)
