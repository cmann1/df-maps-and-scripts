import os
import pickle
import json
from PIL import Image


def get_bounds(img):
	data = img.load()

	x1 = img.size[0]
	y1 = img.size[1]
	x2 = 0
	y2 = 0

	for x in range(0, img.size[0]):
		for y in range(0, img.size[1]):
			if data[x, y][3] != 0:
				if x < x1:
					x1 = x
				if x > x2:
					x2 = x
				if y < y1:
					y1 = y
				if y > y2:
					y2 = y

	return x1, y1, x2, y2


IMG_PATH = 'img/'
IMG_SRC_PATH = 'img_src/'

with open('char-spr-data', 'rb') as f:
	char_data = pickle.load(f)

process_chars = ['dustgirl', 'dustman', 'dustworth', 'dustkid']

json_data = {}

for char_name in process_chars:
	if char_name not in process_chars:
		continue

	json_char_data = {}
	json_data[char_name] = json_char_data

	anims_data = char_data[char_name]
	print('====== Processing char ' + char_name)
	sprite_path = '%s/%s/' % (IMG_PATH, char_name)
	sprite_src_path = '%s/%s/' % (IMG_SRC_PATH, char_name)

	for anim_name, frames in anims_data.items():
		print('   [' + anim_name + ']')
		frame_index = 1
		anim_path = '%s/%s' % (sprite_path, anim_name)
		anim_src_path = '%s/%s' % (sprite_src_path, anim_name)

		json_anim_data = []
		json_char_data[anim_name] = json_anim_data

		for frame in frames:
			frame_path = '%s%04d.png' % (anim_path, frame_index)
			frame_src_path = '%s%04d.png' % (anim_src_path, frame_index)
			# print(frame_path)
			if os.path.exists(frame_path):
				if not os.path.exists(frame_src_path):
					print('Cannot find src file', frame_src_path)
					continue

				img = Image.open(frame_path)
				img_src = Image.open(frame_src_path)
				x, y, x2, y2 = get_bounds(img)
				sx, sy, x3, y3 = get_bounds(img_src)

				json_anim_data.append(sx - x)
				json_anim_data.append(sy - y)

			frame_index += 1
			pass
		pass
	pass

with open('sprite_src_offsets.json', 'w') as f:
	json.dump(json_data, f, indent='\t')
