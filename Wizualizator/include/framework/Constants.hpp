#pragma once
#include <assert.h>

namespace Bio {
	struct HydropathyIndex { float Value; };

	/*
	* Kyte & Doolitle Amino acid scale values
	* Reference: J. Mol. Biol. 157:105-132(1982). 
	*/
	constexpr HydropathyIndex GetHydropathyIndex(const char character) noexcept
	{
		switch (character)
		{
			case 'V': return HydropathyIndex{ .Value = 4.2f };
			case 'A': return HydropathyIndex{ .Value = 1.8f };
			case 'D': return HydropathyIndex{ .Value = -3.5f };
			case 'E': return HydropathyIndex{ .Value = -3.5f };
			case 'G': return HydropathyIndex{ .Value = -0.4f };
			case 'F': return HydropathyIndex{ .Value = 2.8f };
			case 'L': return HydropathyIndex{ .Value = 3.8f };
			case 'S': return HydropathyIndex{ .Value = -0.8f };
			case 'Y': return HydropathyIndex{ .Value = -1.3f };
			case 'C': return HydropathyIndex{ .Value = 2.5f };
			case 'W': return HydropathyIndex{ .Value = -0.9f };
			case 'P': return HydropathyIndex{ .Value = -1.6f };
			case 'H': return HydropathyIndex{ .Value = -3.2f };
			case 'Q': return HydropathyIndex{ .Value = -3.5f };
			case 'R': return HydropathyIndex{ .Value = -4.5f };
			case 'I': return HydropathyIndex{ .Value = 4.5f };
			case 'M': return HydropathyIndex{ .Value = 1.9f };
			case 'T': return HydropathyIndex{ .Value = -0.7f };
			case 'N': return HydropathyIndex{ .Value = -3.5f };
			case 'K': return HydropathyIndex{ .Value = -3.9f };
			case '-': return HydropathyIndex{ .Value = 0.0f };

			[[unlikely]]
			default: 
				assert(false);
				break;
		}

		assert(false);
		return HydropathyIndex{ 0.0f };
	}
}