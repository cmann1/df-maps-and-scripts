class ValentinesTrigger : HolidayTrigger
{
	
	SpriteBatch spr(
		array<string>={'props3','facade_12','props4','machinery_18','props4','machinery_18','props3','facade_12','props4','machinery_19','props4','machinery_19','props3','sidewalk_7','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props3','facade_12','props3','facade_12','props3','facade_12','props4','machinery_19','props4','machinery_19','props3','facade_12','props3','facade_12','props4','machinery_18','props4','machinery_18','props3','sidewalk_7','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props1','study_3','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props3','sidewalk_7','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_18','props4','machinery_18','props4','machinery_18','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props4','machinery_19','props4','machinery_19','props3','sidewalk_7','props4','machinery_19','props4','machinery_19','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props3','sidewalk_7','props4','machinery_18','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','sidewalk_7','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props1','lighting_6','props1','study_3','props3','facade_12','props3','facade_12','props3','facade_12','props3','sidewalk_7','props4','machinery_18','props4','machinery_18','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props1','foliage_11','props1','foliage_11','props1','foliage_11','props3','sidewalk_7','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props4','machinery_18','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props1','study_3','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','lighting_6','props1','foliage_11','props1','foliage_11','props1','foliage_11','props1','study_3','props3','facade_12','props3','facade_12','props3','facade_12','props3','facade_12','props3','sidewalk_6','props3','facade_12','props3','facade_12','props3','facade_12',},
		array<int>={16,17,16,17,16,17,16,17,16,17,16,17,16,16,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,16,17,17,17,17,17,17,17,17,17,17,17,17,17,18,17,19,16,17,16,17,16,17,16,17,16,17,16,17,16,16,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,16,16,17,16,17,16,17,16,17,16,17,16,17,16,16,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,16,16,17,16,17,16,17,16,17,15,7,15,7,15,7,15,7,15,7,15,7,15,8,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,18,17,18,17,19,16,17,16,17,16,17,16,16,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,15,7,15,7,15,7,16,16,15,7,15,7,15,7,15,8,16,17,16,17,16,17,16,17,16,17,16,17,17,17,17,17,17,17,17,17,17,17,17,17,17,18,17,19,16,17,16,17,16,17,16,17,17,17,17,17,17,17,17,17,17,17,17,17,17,18,16,17,16,17,16,17,16,17,16,17,16,17,15,7,15,7,15,7,15,7,15,7,15,7,15,8,15,7,15,7,15,7,15,8,15,7,15,7,15,7,15,9,16,17,16,17,16,17,16,17,16,16,16,17,16,17,16,17,},
		array<float>={-208,-359,-193,-497,-192,-500,-195,-300,-110,-487,-110,-483,-106,-442,-149,-452,-146,-456,-141,-463,-144,-458,-157,-499,-160,-457,-33,-266,-30,-269,-29,-259,-87,-486,-87,-485,17,-355,30,-296,32,-493,33,-496,32,-384,14,-73,11,-82,14,-57,14,-59,8,-71,12,-62,14,-34,14,0,65,-453,68,-495,79,-452,76,-448,84,-459,81,-454,122,-447,114,-364,123,-347,113,-343,105,-297,519,-569,519,-565,542,-568,542,-567,183,-540,183,-536,206,-539,206,-538,327,-540,328,-543,363,-542,260,-319,263,-322,264,-312,201,-274,207,-269,210,-266,338,-502,338,-498,323,-443,361,-501,361,-500,373,-500,378,-507,375,-502,359,-501,417,-511,370,-496,312,-402,339,-360,338,-339,325,-343,348,-343,330,-293,415,-281,418,-284,477,-408,494,-327,503,-319,500,-322,419,-274,445,-107,442,-96,448,-98,446,-87,448,-82,448,-84,448,-59,529,43,529,41,529,27,523,29,527,38,526,18,483,86,483,84,483,70,477,72,481,81,480,61,483,109,529,66,529,100,596,-348,599,-351,600,-341,660,-473,660,-437,661,-440,643,-390,649,-289,655,-284,645,-299,658,-281,658,-240,634,-407,633,-386,625,-340,747,-111,749,-106,749,-108,750,-412,746,-131,749,-122,743,-120,749,-83,696,-439,703,-392,706,-396,711,-403,708,-398,692,-397,721,41,721,39,721,25,715,27,719,36,718,16,721,64,721,98,763,-332,763,-328,786,-331,786,-330,773,85,773,83,773,69,767,71,771,80,770,60,773,108,839,-348,836,-351,840,-111,843,-114,844,-104,830,-356,869,-135,866,-124,870,-115,872,-126,872,-110,872,-112,872,-87,363,-82,360,-71,366,-73,366,-34,366,-57,364,-62,366,-59,366,0,967,-304,966,-283,976,-287,958,-237,994,-119,1074,-119,1080,-114,1083,-111,},
		array<float>={-0.721804,0.721804,0.721804,0.721804,0.721804,0.721804,0.613238,0.613238,1,1,1,1,1,1,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.721804,0.721804,0.613238,0.613238,-0.521001,0.521001,-0.521001,0.521001,-0.521001,0.521001,1,1,1,1,-0.721804,0.721804,0.613238,0.613238,0.721804,0.721804,0.721804,0.721804,1,1,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,1,1,0.613238,0.613238,0.721804,0.721804,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,1,1,0.721804,0.721804,0.721804,0.721804,0.721804,0.721804,-0.613238,0.613238,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.721804,0.721804,0.721804,0.721804,0.721804,0.721804,-0.521001,0.521001,-0.521001,0.521001,-0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,1,1,1,1,1,1,1,1,1,1,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,1,1,0.613238,0.613238,-0.721804,0.721804,0.721804,0.721804,0.721804,0.721804,0.613238,0.613238,0.721804,0.721804,-0.613238,0.613238,-0.521001,0.521001,-0.521001,0.521001,1,1,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,-0.521001,0.521001,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,1.17704,1.17704,1,1,-0.521001,0.521001,-0.521001,0.521001,-0.521001,0.521001,1,1,0.721804,0.721804,0.721804,0.721804,0.721804,0.721804,0.521001,0.521001,0.521001,0.521001,-0.721804,0.721804,0.521001,0.521001,0.613238,0.613238,0.721804,0.721804,0.721804,0.721804,-0.613238,0.613238,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1,1,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,0.721804,0.721804,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.613238,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,1,1,1,1,1,1,1,1,1,1,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,0.521001,0.521001,0.521001,0.521001,-0.521001,0.521001,-0.521001,0.521001,-0.521001,0.521001,0.521001,0.521001,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1.17704,1.17704,0.84959,0.84959,0.84959,0.84959,0.84959,0.84959,1,1,0.721804,0.721804,0.721804,0.721804,0.721804,0.721804,-0.613238,0.613238,1,1,0.521001,0.521001,0.521001,0.521001,0.521001,0.521001,},
		array<float>={241.957,1.95557,1.95557,320.537,0,0,90,0,0,0,0,1.95557,0,239.996,239.996,232.861,0,0,241.957,320.537,1.95557,1.95557,90,19.9951,0,9.99756,319.999,339.999,349.997,180,180,0,1.95557,0,0,0,0,90,116.603,121.959,126.744,40.5341,0,0,0,0,0,0,0,0,1.95557,1.95557,1.95557,239.996,239.996,232.861,119.998,119.998,119.998,0,0,90,0,0,0,0,0,0,90,0,241.957,116.603,126.744,320.537,121.959,40.5341,239.996,239.996,90,119.998,119.998,119.998,232.861,0,339.999,19.9951,349.997,9.99756,319.999,180,9.99756,319.999,19.9951,339.999,349.997,0,9.99756,319.999,19.9951,339.999,349.997,0,180,180,180,239.996,239.996,232.861,90,1.95557,1.95557,121.959,119.998,119.998,241.957,119.998,320.537,116.603,126.744,40.5341,349.997,9.99756,319.999,90,0,19.9951,339.999,180,1.95557,0,0,0,0,0,9.99756,319.999,19.9951,339.999,349.997,0,180,180,0,0,0,0,9.99756,319.999,19.9951,339.999,349.997,0,180,119.998,119.998,239.996,239.996,232.861,119.998,0,339.999,349.997,19.9951,9.99756,319.999,180,0,339.999,19.9951,180,9.99756,349.997,319.999,180,116.603,126.744,121.959,40.5341,259.997,119.998,119.998,119.998,},
		array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,});
	
	ValentinesTrigger()
	{
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		spr.draw(x, y);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}