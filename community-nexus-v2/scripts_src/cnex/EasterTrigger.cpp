class EasterTrigger : HolidayTrigger
{

	SpriteBatch spr(
		array<string>={'props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_18','props4','machinery_18','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props2','leaves_8','props4','machinery_19','props4','machinery_18','props4','machinery_18','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_1','props1','foliage_13','props1','foliage_10','props1','foliage_1','props1','foliage_1','props1','foliage_11','props2','foliage_10','props1','foliage_1','props1','foliage_1','props1','foliage_1','props1','foliage_9','props1','foliage_13','props2','foliage_4','props1','foliage_1','props2','leaves_2','props1','foliage_1','props1','foliage_1','props2','foliage_4','props1','foliage_1','props1','foliage_9','props1','foliage_1','props1','foliage_10','props1','foliage_1','props1','foliage_9','props1','foliage_9','props1','foliage_1','props1','foliage_1','props1','foliage_10','props1','foliage_1','props1','foliage_10','props2','foliage_4','props1','foliage_1','props2','foliage_4','props1','foliage_1','props1','foliage_1','props1','foliage_1','props1','foliage_1','props1','foliage_1','props2','foliage_10','props2','foliage_11','props1','foliage_1','props1','foliage_13','props1','foliage_1','props1','foliage_1','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_18','props4','machinery_18','props1','foliage_1','props1','foliage_1','props1','foliage_1','props2','leaves_8','props2','foliage_11','props2','foliage_10','props1','foliage_1','props1','foliage_1','props1','foliage_1','props2','foliage_10','props1','foliage_1','props1','foliage_1','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_13','props1','foliage_1','props1','foliage_13','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_1','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_18','props4','machinery_18','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_1','props1','foliage_9','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_1','props1','foliage_10','props2','leaves_2','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_1','props1','foliage_1','props1','foliage_9','props1','foliage_1','props1','foliage_1','props1','foliage_13','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_19','props4','machinery_18','props4','machinery_19','props4','machinery_18','props4','machinery_19','props4','machinery_19','props4','machinery_19','props1','foliage_1','props2','foliage_4','props1','foliage_9',},
		array<int>={16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,2,16,2,16,2,16,2,16,6,16,6,16,6,16,6,16,6,16,2,16,2,16,2,16,2,16,2,14,19,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,15,19,14,19,16,19,15,8,16,19,16,19,15,5,15,19,15,19,15,8,14,19,14,19,17,19,15,8,14,19,15,8,17,19,17,19,16,19,17,19,16,19,17,19,16,19,16,19,15,18,15,19,15,19,15,18,15,19,16,19,15,18,16,19,16,19,16,19,16,19,15,19,16,19,16,19,15,19,15,19,14,19,14,19,14,19,14,19,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,14,19,14,19,14,19,14,19,15,19,15,19,16,19,15,8,15,8,15,19,16,19,14,19,16,0,16,0,16,0,16,0,16,0,16,0,16,0,16,0,16,0,14,19,16,19,14,19,16,4,16,4,16,4,16,4,16,4,16,4,16,4,16,4,16,4,17,19,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,6,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,17,19,14,19,16,6,16,6,16,6,16,6,16,6,16,6,16,6,15,8,14,19,14,19,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,15,8,15,8,14,19,15,8,15,8,14,19,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,2,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,16,8,15,19,15,18,15,18,},
		array<float>={-330,-243,-329,-249,-322,-265,-325,-259,-318,-261,-314,-257,-328,-254,-309,-245,-306,-241,-307,-246,-310,-251,-314,-256,-311,-250,-331,-238,-304,-235,-331,-231,-319,-224,-314,-225,-257,-272,-255,-277,-254,-272,-253,-273,-199,-283,-196,-288,-193,-293,-203,-272,-201,-278,-258,-264,-258,-268,-254,-262,-252,-268,-252,-264,-256,-234,-176,-267,-189,-260,-194,-261,-177,-285,-179,-290,-178,-284,-179,-291,-184,-301,-181,-297,-189,-297,-176,-278,-176,-273,-178,-279,-102,-381,-132,-51,-117,-12,-104,-13,-75,9,-77,-2,-89,-332,-159,-308,-44,-390,-26,-2,-30,-33,10,6,55,17,61,-29,66,-621,148,-6,445,115,381,106,491,98,543,91,568,96,692,96,706,91,491,72,839,86,844,114,894,73,942,51,935,98,710,49,910,43,429,92,464,91,342,66,212,-425,177,-427,250,-420,275,-439,252,-262,276,-261,260,-225,195,-15,194,-72,245,-140,278,-99,281,-94,275,-103,266,-112,266,-113,270,-107,273,-103,271,-108,261,-116,256,-119,254,-112,253,-107,253,-101,254,-95,256,-84,254,-90,269,-80,274,-81,279,-182,307,-103,318,-49,227,-303,240,-330,276,-317,295,-390,228,-366,295,-355,384,-358,296,15,341,-31,365,-193,362,-193,364,-198,366,-194,365,-183,361,-185,367,-189,361,-189,367,-185,381,-2,343,-422,508,7,585,-162,587,-158,586,-147,582,-149,588,-153,582,-153,586,-157,583,-157,588,-149,577,-44,636,-213,635,-213,615,-213,612,-209,637,-207,636,-202,636,-196,635,-207,623,-190,618,-191,610,-203,641,-89,638,-89,638,-94,641,-91,643,-80,639,-81,644,-86,638,-84,645,-82,676,-14,679,-127,635,-219,635,-220,621,-223,618,-218,631,-230,634,-225,626,-227,737,-90,705,-113,735,-613,794,-195,792,-180,795,-186,794,-190,795,-191,794,-182,791,-191,788,-183,789,-187,814,-435,844,-86,829,-106,854,-391,914,-120,904,-129,283.306,-242.558,277.673,-234.924,282.712,-237.493,279.86,-238.42,283.379,-233.071,283.972,-238.136,276.438,-231.119,282.146,-229.267,279.624,-227.982,276.626,-263.142,277.911,-263.736,277.16,-270.402,276.817,-269.463,276.065,-276.13,273.685,-281.262,275.811,-246.398,277.72,-257.414,276.939,-252.376,275.845,-258.102,268.21,-277.945,263.671,-274.284,262.387,-241.736,260.072,-270.278,257.691,-243.458,256.129,-265.334,250.463,-255.695,253.468,-260.985,968,86,979,88,956,57,},
		array<float>={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,1,1,1,1,1,-1,1,1,1,-1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,},
		array<float>={352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,352.271,0,0,0,0,10.6677,10.6677,10.6677,10.6677,10.6677,0,0,0,0,0,319.999,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,10.6677,239.996,339.999,0,119.998,90,0,180,209.998,199.995,79.9969,0,329.996,329.996,39.9957,29.9982,99.9976,99.9976,339.999,79.9969,339.999,69.9994,19.9951,169.997,339.999,339.999,109.995,59.9963,9.99756,90,29.9982,339.999,90,-30,120,329.996,329.996,309.996,349.997,189.998,159.999,19.9951,299.998,19.9951,39.9957,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,338.978,189.998,159.999,209.998,319.999,219.996,159.999,180,349.997,0,180,96.806,139.999,0,0,0,0,0,0,0,0,0,339.999,249.999,29.9982,0,0,0,0,0,0,0,0,0,19.9951,15.65,15.65,15.65,15.65,15.65,15.65,15.65,15.65,15.65,15.65,15.65,343.367,343.367,343.367,343.367,343.367,343.367,343.367,343.367,343.367,139.999,19.9951,15.65,15.65,15.65,15.65,15.65,15.65,15.65,119.998,19.9951,29.9982,13.2935,13.2935,13.2935,13.2935,13.2935,13.2935,13.2935,13.2935,13.2935,90,99.9976,349.997,139.999,69.9994,9.99756,17.985,17.985,17.985,17.985,17.985,17.985,17.985,17.985,17.985,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,20.143,129.996,9.99756,9.99756,},
		array<uint>={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,});
	
	EasterTrigger()
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