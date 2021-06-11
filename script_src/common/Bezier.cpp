class Bezier
{
	
	float x1, y1;
	float x2,  y2;
	float x3, y3;
	float x4, y4;
	
	int num_arcs;
	array<float> arc_lengths(num_arcs + 1);
	float length;
	
	Bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int num_arcs=10)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		this.x3 = x3;
		this.y3 = y3;
		this.x4 = x4;
		this.y4 = y4;
		this.num_arcs = num_arcs;
	}
	
	void update()
	{
		arc_lengths[0] = 0;
		
		float ox = x(0);
		float oy = y(0);
		float clen = 0;
		const float tx = 1.0 / num_arcs;
		
		for(int i = 1; i <= num_arcs; i++) {
			float px = x(i * tx);
			float py = y(i * tx);
			float dx = ox - px;
			float dy = oy - py;        
			clen += sqrt(dx * dx + dy * dy);
			arc_lengths[i] = clen;
			ox = px;
			oy = py;
		}
		
		length = clen;
	}
	
	float x(float t)
	{
		const float t1 = 1 - t;
		return (t1 * t1 * t1) * x1
			+ 3 * (t1 * t1) * t * x2
			+ 3 * t1 * (t * t) * x3
			+ (t * t * t) * x4;
    }

    float y(float t)
	{
		const float t1 = 1 - t;
		return (t1 * t1 * t1) * y1
			+ 3 * (t1 * t1) * t * y2
			+ 3 * t1 * (t * t) * y3
			+ (t * t * t) * y4;
    }
	
	float map(float distance)
	{
        int low = 0, high = num_arcs;
		int index = 0;
        while(low < high)
		{
            index = low + (((high - low) / 2) | 0);
            if(arc_lengths[index] < distance)
			{
                low = index + 1;
            }
			else
			{
                high = index;
            }
        }
        if(arc_lengths[index] > distance)
		{
            index--;
        }

        float lengthBefore = arc_lengths[index];
        if(lengthBefore == distance)
		{
            return float(index) / num_arcs;

        } else {
            return (index + (distance - lengthBefore) / (arc_lengths[index + 1] - lengthBefore)) / num_arcs;
        }
    }
	
	float mx(float distance)
	{
        return x(map(distance));
    }

    float my(float distance)
	{
        return y(map(distance));
    }
	
}