namespace StoneBlock
{
	
	const int ColType = 100;
	
	const int TypeLarge = 0;
	const int TypeSmall = 1;
	const int TypePillar = 2;
	const int TypePillarChunk = 3;
	const int TypeHeadLarge = 4;
	const int TypeHeadSmall = 5;
	const int TypeCrateLarge = 6;
	const int TypeCrateSmall = 7;
	const int TypeBarrel = 8;
	
	const float BaseSize = 68;
	
	const float SegmentSpacing = 34;
	const int MaxSegments = 8;
	
	const float ConstraintStiffness = 0.95;
	const float ConstraintDamping = 0.25;
	const float SurfaceFriction = 150;
	const float AttackForceMultiplier = 3;
	const float ContactMass = 0.25;
	const float ContactVelocityMultipler = 45;
	
	const int Layer = 18;
	const int SubLayer = 22;
	
	const float HeavyLandMin = 7;
	const float HeavyLandMax = 10;
	const float HeavyLandShake = 100;
	
	const int CollisionEmitterId = EmitterId::SlimeRoofCreate;
	const int HeavyCollisionEmitterId = EmitterId::SlimeGroundCreate;
	const int CollisionEmitterSubLayer = 7;
	const float LandMin = 1.5;
	
	void set_sprites(SpriteData@ spr_cache, StoneBlock@ block)
	{
		block.marking_alpha = 0.25;
		
		switch(block.type)
		{
			case TypeSmall:
				@block.shadow_spr		= @spr_cache.small_stone_shadow_spr;
				@block.attachment_spr	= @spr_cache.small_stone_attachment_spr;
				@block.block_spr		= @spr_cache.small_stone_spr;
				@block.markings_spr		= @spr_cache.small_stone_markings_spr;
				break;
			case TypePillar:
				@block.shadow_spr		= @spr_cache.pillar_shadow_spr;
				@block.attachment_spr	= null;
				@block.block_spr		= @spr_cache.pillar_spr;
				@block.markings_spr		= @spr_cache.pillar_markings_spr;
				block.marking_alpha = 0.5;
				break;
			case TypePillarChunk:
				@block.shadow_spr		= @spr_cache.pillar_chunk_shadow_spr;
				@block.attachment_spr	= null;
				@block.block_spr		= @spr_cache.pillar_chunk_spr;
				@block.markings_spr		= @spr_cache.pillar_chunk_markings_spr;
				block.marking_alpha = 0.5;
				break;
			case TypeHeadLarge:
				@block.shadow_spr		= @spr_cache.head_large_shadow_spr;
				@block.attachment_spr	= @spr_cache.head_large_attachment_spr;
				@block.block_spr		= @spr_cache.head_large_spr;
				@block.markings_spr		= null;
				break;
			case TypeHeadSmall:
				@block.shadow_spr		= @spr_cache.head_small_shadow_spr;
				@block.attachment_spr	= @spr_cache.head_small_attachment_spr;
				@block.block_spr		= @spr_cache.head_small_spr;
				@block.markings_spr		= null;
				break;
			case TypeCrateLarge:
				@block.shadow_spr		= @spr_cache.crate_large_shadow_spr;
				@block.attachment_spr	= @spr_cache.crate_large_attachment_spr;
				@block.block_spr		= @spr_cache.crate_large_spr;
				@block.markings_spr		= null;
				break;
			case TypeCrateSmall:
				@block.shadow_spr		= @spr_cache.crate_small_shadow_spr;
				@block.attachment_spr	= @spr_cache.crate_small_attachment_spr;
				@block.block_spr		= @spr_cache.crate_small_spr;
				@block.markings_spr		= null;
				break;
			case TypeBarrel:
				@block.shadow_spr		= @spr_cache.barrel_shadow_spr;
				@block.attachment_spr	= @spr_cache.barrel_attachment_spr;
				@block.block_spr		= @spr_cache.barrel_spr;
				@block.markings_spr		= null;
				break;
			case TypeLarge:
			default:
				@block.shadow_spr		= @spr_cache.large_stone_shadow_spr;
				@block.attachment_spr	= @spr_cache.large_stone_attachment_spr;
				@block.block_spr		= @spr_cache.large_stone_spr;
				@block.markings_spr		= @spr_cache.large_stone_markings_spr;
				break;
		}
	}
	
	void set_base_size(StoneBlock@ block)
	{
		float density = 1;
		
		switch(block.type)
		{
			case TypeSmall:
				block.size_x = 48;
				block.size_y = 48;
				block.attachment_offset = 22;
				break;
			case TypePillar:
				block.size_x = 20;
				block.size_y = 140;
				block.attachment_offset = 0;
				break;
			case TypePillarChunk:
				block.size_x = 47;
				block.size_y = 27;
				block.attachment_offset = 0;
				break;
			case TypeHeadLarge:
				block.size_x = 54;
				block.size_y = 48;
				block.attachment_offset = 27.5;
				break;
			case TypeHeadSmall:
				block.size_x = 30;
				block.size_y = 30;
				block.attachment_offset = 16;
				break;
			case TypeCrateLarge:
				block.size_x = 56;
				block.size_y = 46;
				block.attachment_offset = 13;
				density = 0.35;
				break;
			case TypeCrateSmall:
				block.size_x = 31;
				block.size_y = 31;
				block.attachment_offset = 8.5;
				density = 0.35;
				break;
			case TypeBarrel:
				block.size_x = 26;
				block.size_y = 41;
				block.attachment_offset = 9.5;
				density = 0.4;
				break;
			case TypeLarge:
			default:
				block.size_x = BaseSize;
				block.size_y = BaseSize;
				block.attachment_offset = 32;
				break;
		}
		
		block.mass = max(
			(block.size_x * block.scale * block.size_y * block.scale) /
				(BaseSize * BaseSize) * density,
			0.01);
		block.update_rect();
	}
	
}
