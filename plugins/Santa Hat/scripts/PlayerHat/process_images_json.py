import pickle
import json
import os

import math
from PIL import Image

# Outputs the hat data as json, but relative to the source artwork files, instead of the compiled and trimmed sprites in the game.
# get_src_sprite_offsets.py needs to be run first to calculate the offsets

IMG_PATH = 'img/'

with open('char-spr-data', 'rb') as f:
	char_data = pickle.load(f)
with open('sprite_src_offsets.json') as fp:
	spr_offsets = json.load(fp)

RED = (255, 0, 0, 255)
GREEN = (0, 255, 0, 255)

process_chars = ['dustman', 'dustgirl', 'dustworth', 'dustkid']

json_data = {}

for char_name in char_data:
	if char_name not in process_chars: continue

	json_char_data = {}
	json_data[char_name] = json_char_data

	offset_char_data = spr_offsets[char_name]

	anims_data = char_data[char_name]
	print('====== Processing char ' + char_name)
	sprite_path = '%s/%s/' % (IMG_PATH, char_name)

	for anim_name, frames in anims_data.items():
		print('   [' + anim_name + ']')
		frame_index = 1
		anim_path = '%s/%s' % (sprite_path, anim_name)
		prev_x, prev_y, prev_angle = 0, 0, 0

		json_anim_data = []
		json_char_data[anim_name] = json_anim_data

		anim_offset_data = offset_char_data[anim_name]

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
							start = (x + 0.5, y + 0.5)
							if end is not None:
								must_break = True
								break
						elif pixdata[x, y] == GREEN:
							end = (x + 0.5, y + 0.5)
							if start is not None:
								must_break = True
								break
					if must_break: break

				if start is None or end is None:
					missing_data.append('Missing data for ' + frame_path)

			if start is not None and end is not None:
				i = (frame_index - 1) * 2

				dx = end[0] - start[0]
				dy = end[1] - start[1]

				mid_x = start[0] + dx * 0.5 + anim_offset_data[i]
				mid_y = start[1] + dy * 0.5 + anim_offset_data[i + 1]
				# mid_x = start[0] + dx * 0.5 + frame['x'] + 96
				# mid_y = start[1] + dy * 0.5 + frame['y'] + 192

				json_anim_data.append(mid_x)
				json_anim_data.append(mid_y)
				json_anim_data.append(math.atan2(dy, dx))

				processed_frames += 1

			frame_index += 1
			# break

		if processed_frames > 0:
			for msg in missing_data:
				print(msg)

# out_dir = 'C:/Users/Carl/AppData/Roaming/Dustforce/user/script_src/mod_sh/'
out_dir = ''
with open(out_dir + 'hat_data.json', 'w') as f:
	json.dump(json_data, f, indent='\t')
