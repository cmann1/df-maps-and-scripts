import pickle
import json


SINGLE_TEST_CHAR_DATA = False
SINGLE_TEST_ANIM_DATA = False

with open('../dustforce_sprites_reference/sprites-reference-data', 'rb') as f:
	prop_data = pickle.load(f)

characters_data = dict()
old_data = dict()
for key, value in prop_data.items():
	if key in ['dustman', 'dustgirl', 'dustworth', 'dustkid']:
		anims = prop_data[key]
		formatted_anims = dict()

		for anim_name, value in anims.items():
			if anim_name[0:2] in ['dm', 'dk', 'do', 'dg']: continue

			anim_data = anims[anim_name]['sprites'][-1]
			frames_data = anim_data['palettes'][0]
			frames = []

			for frame_data in frames_data:
				rect = frame_data['rect']
				frames.append(dict(x=rect[0], y=rect[1], w=rect[2], h=rect[3]))

			formatted_anims[anim_name] = frames
			if SINGLE_TEST_ANIM_DATA: break

		characters_data[key] = formatted_anims
		old_data[key] = anims
		if SINGLE_TEST_CHAR_DATA: break

with open('old-spr-data.json', 'w') as f:
	json.dump(old_data, f, indent='\t')
with open('char-spr-data.json', 'w') as f:
	json.dump(characters_data, f, indent='\t')
with open('char-spr-data', 'wb') as f:
	pickle.dump(characters_data, f)
