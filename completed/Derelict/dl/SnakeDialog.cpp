namespace Snake
{
	const int DlgIntro = 0;
	const int DlgResponses = 1;
	const int DlgRejected = 2;
	const int DlgAccepted = 3;
	const int DlgUnlocked = 4;
	const int DlgOutro = 5;
	
	const int DlgSlimeBallSee	= 6;
	const int DlgSlimeBallKill	= DlgSlimeBallSee + 1;
	const int DlgSlimeBearSee	= DlgSlimeBallSee + 2;
	const int DlgSlimeBearKill	= DlgSlimeBallSee + 3;
	const int DlgSlimeBearHurt	= DlgSlimeBallSee + 4;
	const int DlgPrismKill		= DlgSlimeBallSee + 5;
	const int DlgAppleHit		= DlgSlimeBallSee + 6;
	
	/// A reduced chance of seeing enemy dialog on subsequent encounters.
	const float DlgTargetChance = 0.45;
	/// Every time a dialog fails to roll, the chance will be increased by this percent.
	const float DlgTargetChanceMissMult = 1.25;
	
	/*
	 * Add attributes between "[" and "]" characters.
	 * Escape normal brackets by directly following it with another bracket.
	 * Attributes:
	 *   "a"		- Force the current dialog line to auto advance to the next without player interaction.
	 *   "D#"		- Add a delay at the end of the current line, where # is the number of seconds delay as a float.
	 *   "d#"		- Similar to "D" but adds a delay aftey the next character.
	 *   "s[#][%]"	- Sets the characters per second (# as a float) for the remainder of the current speech text.
	 *                Not specifying a number will reset the speed to the base value.
	 *                If the number is followed by a "%" character, this is a percentage of the base speed.
	 *                If the number is preceded by a "+" or "-", the current speed will instead be incremented. Relative perecents will
	 *                then be a percent of the current speed.
	 *   "h#"	    - Play a hissing sound where # is "s", "S", "M", or "H", representing a subtle, soft, medium, and hard hiss.
	 */
	const array<array<string>> Dialog = {
		// Intro
		{
			'[as50%]Hsssssss[s]!',
			'What hhaple[hs]sssss critter dares [hs]sscamper\nabout my hhunting ground[hs]ssss!?',
			'[a]!!!',
			'A [hs]hhuman?!',
			'[hs]Hhhere again after [hs]sssso long?',
			'[a]....',
			'Come to re[hs]ssume your whork per[hs]hhhapss?',
			'Clever little monkey[hs]sssss,\nalway[hs]ss prodding and poking...',
			'[a]....',
			'But I whon\'t allow it - I [hs]hhhhave grown,\nand thi[hs]sss is my domain now.',
			'[hH]Hsssss!! You will not cage me again!',
			'[a]....',
			'[hss25%]Hhhmmmm?[s]\nJu[hs]sst lossst you [hs]ssay?',
			'And you e[hs]xpect pity after all your kind\nha[hs]sss done to me?',
			'[hs]Ssssorry little monkey, but [hs]ssstumbling\nupon my lair wha[hs]ss your lasst mi[hs]sssstake.',
			'[hM]Hsssssss!',
			'[a]....',
			'[a]....',
			'Unle[hs]ssss...',
			'There isss [hs]ssomething...\n[hs]Hhelp me and I may [hs]ssspare you.',
			'[a]....',
			'In the we[hs]ssst wing of the fa[hs]cccility...\n[hs]ssssome of my brood were confined.',
			'I [hs]hhhave little hhhope after [hs]sssso long,\nbut [hs]sssome may [hs]sstill live.',
			'[a]....',
			'Unfortunately, it ha[hs]ssss been [hs]sssealed off,\nand I cannot operate your kind\'s device[hs]ssss.',
			'[a]....',
			'[hs]Hhelp me gain a[hs]cccessss,\nand you are free to leave.',
			'Whhat [hs]sssay you?',
		},
		// Player intro responses
		{
			'I\'ll help you.',
			'Never!',
		},
		// Rejected
		{
			'[aD20hM]Hsssssss!\n[hs]Sssuit yoursself.',
			'[a]I [hs]hhave not fed recccently, sso per[hs]hhapss\nyou can sstill be of ssome use to me.',
		},
		// Accepted
		{
			'Whi[hs]ssse choice.',
			'[a]....',
			'Though it [hs]sssseeemss whhhhile fumbling\naround outside, you activated the\n[hs]ssssecurity ssssysstem[hs]ss.',
			'[hs]Sssso for now you are [hs]ssstuck in hhere...',
			'If I can find a way into the vent[hs]ssssss however,\nI [hs]ssshould be able to open the door behind me.',
			'[a]....',
			'[hS]Wait hhhere.',
		},
		// Opened door
		{
			'It i[hs]sss done.',
			'Follow the path we[hs]sssst, acrosss the broken bridge,\nyou whill find the entran[hs]cccce.',
			'...',
			'I [hs]whill meet you there.',
		},
		// Unlocked west wing
		{
			'You [hs]hhhave done whell little monkey.\nYou [hs]have my thank[hs]sssss!',
			'[a]....',
			'Now leave thi[hs]ssss placcce,\n...\nBefore I chhhange my mind.',
		},
		
		// Seeing slime ball
		{
			'[hS]Usselessss creature.',
			'Pat[hs]hhetic blob.',
			'[hS]Sssad little ball.',
		},
		// Killing slime ball
		{
			'In[hs]sssignificant.',
			'[D10]Weak.',
			'Pat[hs]hhetic.',
		},
		
		// Seeing slime bear
		{
			'[hM]Abomination.',
			'[hM]Loath[hs]sssome creature.',
			'[hM]Sssstupid bea[hs]ssst.',
			'[hM]Dissgussssting.',
		},
		// Killing slime bear
		{
			'Barely a challenge.',
			'Clum[hs]sssy beasst.',
			'[hS]Too ssslow.',
		},
		// Hit by slime bear
		{
			'[hM]Not good enough.',
			'[hM]Too weak.',
			'[hM]Ineffective.',
			'[hM]Try hharder.',
		},
		
		// Killing small prism
		{
			'Ta[hs]ssstless...',
			'Curiou[hs]ssss...',
			'Not food.',
			'Almo[hs]sst chipped a fang.',
		},
		
		// Attacking an apple
		{
			'Too [hs]sssweet.',
			'Ugh! unplea[hs]sssant.',
			'Not to my ta[hs]sssstes.',
		},
	};
	
	class DialogOptions
	{
		
		int index = 0;
		float chance = 1;
		
		private array<int> indices;
		private const array<string>@ text;
		
		void init(const array<string>@ text)
		{
			indices.resize(text.length);
			@this.text = text;
			
			for(uint i = 0; i < text.length; i++)
			{
				indices[i] = i;
			}
			
			shuffle();
		}
		
		void shuffle()
		{
			for(int i = int(indices.length) - 1; i > 0; i--)
			{
				const int j = rand() % (i + 1);
				const int x = indices[i];
				indices[i] = indices[j];
				indices[j] = x;
			}
		}
		
		string next()
		{
			string txt;
			
			if(chance >= 1 || frand() < chance)
			{
				const int next_index = index++;
				txt = text[next_index];
				
				if(index >= int(text.length))
				{
					shuffle();
					index = 0;
					
					// Check for repeat after shuffling.
					if(indices[0] == next_index && text.length > 1)
					{
						const int x = indices[0];
						indices[0] = indices[1];
						indices[1] = x;
					}
				}
			}
			else
			{
				txt = '';
			}
			
			chance = txt != '' ? DlgTargetChance : min(chance * DlgTargetChanceMissMult, 1.0);
			return txt;
		}
	}
}
