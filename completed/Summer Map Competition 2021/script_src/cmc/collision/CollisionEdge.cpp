#include '../../lib/enums/EntityState.cpp';

#include 'ICollisionEntity.cpp';
#include 'SideType.cpp';
#include 'EdgeContact.cpp';

class CollisionEdge
{
	
	ICollisionEntity@ entity;
	float x1, y1, x2, y2;
	float dx, dy;
	float normal_x, normal_y;
	float length;
	float bounds_x1, bounds_y1, bounds_x2, bounds_y2;
	int side;
	string surface_type = '';
	uint8 mask = 0xff;
	float add_velocity = 0.25;
	
	int size_contacts = 8;
	int num_contacts;
	array<EdgeContact> contacts(size_contacts);
	
	int size_contacts_leave = 8;
	int num_contacts_leave;
	array<EdgeContact@> contacts_leave(size_contacts_leave);
	
	int angle;
	
	void update()
	{
		num_contacts_leave = 0;
		
		dx = (x2 - x1);
		dy = (y2 - y1);
		float normal_x =  dy;
		float normal_y = -dx;
		length = sqrt(normal_x * normal_x + normal_y * normal_y);
		normal_x = length != 0 ? normal_x / length : 0;
		normal_y = length != 0 ? normal_y / length : -1;

		angle = int(round(atan2(normal_x, -normal_y) * RAD2DEG));

		if(angle <= Collision::GroundSlopeMax && angle >= -Collision::GroundSlopeMax)
			side = SideType::Ground;
		else if(angle > Collision::WallSlantDownMax || angle < -Collision::WallSlantDownMax)
			side = SideType::Roof;
		else if(angle > 0)
			side = SideType::Left;
		else
			side = SideType::Right;
		
		if(x1 <= x2)
		{
			bounds_x1 = x1;
			bounds_x2 = x2;
		}
		else
		{
			bounds_x1 = x2;
			bounds_x2 = x1;
		}
		
		if(y1 <= y2)
		{
			bounds_y1 = y1;
			bounds_y2 = y2;
		}
		else
		{
			bounds_y1 = y2;
			bounds_y2 = y1;
		}
		
		for(int i = num_contacts - 1; i >= 0; i--)
		{
			EdgeContact@ e = @contacts[i];
			controllable@ c = e.data.c;
			
			if(
				@e.data.collision_roof != @this && @e.data.collision_ground != @this &&
				@e.data.collision_left != @this && @e.data.collision_right != @this)
			{
				if(add_velocity > 0)
				{
					c.set_speed_xy(
						c.x_speed() + e.dx / DT * add_velocity,
						c.y_speed() + e.dy / DT * add_velocity);
				}
				
				if(num_contacts_leave + 1 >= size_contacts_leave)
				{
					contacts_leave.resize(size_contacts_leave *= 2);
				}
				
				@contacts_leave[num_contacts_leave++] = e;
				contacts[i] = contacts[--num_contacts];
				continue;
			}
			
			e.has_contact = false;
			
			// Skip if not holding/standing on surface
			switch(side)
			{
				case SideType::Ground:
					if(!c.ground()) continue;
					break;
				case SideType::Roof:
					if(!c.roof()) continue;
					break;
				case SideType::Left:
					if(!c.wall_left()) continue;
					if(c.state() < EntityState::WallRun || c.state() > EntityState::WallGrabIdle) continue;
					break;
				case SideType::Right:
					if(!c.wall_right()) continue;
					if(c.state() < EntityState::WallRun || c.state() > EntityState::WallGrabIdle) continue;
					break;
			}
			
			// Calculate the contact point delta since the last frame
			const float new_x = x1 + dx * e.t;
			const float new_y = y1 + dy * e.t;
			e.dx = new_x - e.x;
			e.dy = new_y - e.y;
			e.data.contact_count++;
			//c.set_xy(
			//	c.x() + e.dx,
			//	c.y() + e.dy);
			e.data.contact_dx += e.dx;
			e.data.contact_dy += e.dy;
			e.x = new_x;
			e.y = new_y;
			e.has_contact = true;
		}
	}
	
	void add_contact(CollisionData@ data, const float x, const float y, const float t)
	{
		EdgeContact@ ec = null;
		
		for(int i = num_contacts - 1; i >= 0; i--)
		{
			if(@contacts[i].data == @data)
			{
				@ec = @contacts[i];
				break;
			}
		}
		
		bool first_contact = @ec == null;
		
		if(first_contact)
		{
			if(num_contacts + 1 >= size_contacts)
			{
				contacts.resize(size_contacts *= 2);
			}
			
			@ec = @contacts[num_contacts++];
		}
		
		
		@ec.data = data;
		ec.x = x;
		ec.y = y;
		ec.t = t;
		
		if(first_contact)
		{
			ec.speed_x = data.c.x_speed();
			ec.speed_y = data.c.y_speed();
		}
	}
	
	void clear()
	{
		num_contacts = 0;
	}
	
}
