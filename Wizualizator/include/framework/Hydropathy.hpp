#pragma once
#include "Nucleotides.hpp"
#include "Constants.hpp"

namespace Bio
{
	struct HydropathyPlotData
	{
		std::vector<double> hydropathyIndices;
		double maxScore, minScore;
	};

	inline bool GenerateHydropathyPlotData(const Bio::AminoSequence& sequence, const size_t window, HydropathyPlotData& data)
	{
		if (!(sequence.size() / 2U > window)) // plot data can't be generated
			return false;

		double sumOfAll{ 0.0 };

		size_t indexCount = sequence.size() - window + 1U;

		data.hydropathyIndices.resize(indexCount);
		data.maxScore = 0;
		data.minScore = 0;

		for (size_t i = 0; i < indexCount; ++i)
		{
			for (size_t pos{ i }; pos < window + i; ++pos)
			{
				Bio::AminoAcid aminoAcid = sequence[pos];

				data.hydropathyIndices[i] += GetHydropathyIndex(aminoAcid.AsCharacter()).Value;
			}

			data.hydropathyIndices[i] /= window;
			sumOfAll += data.hydropathyIndices[i];

			if (data.hydropathyIndices[i] > data.maxScore)
				data.maxScore = data.hydropathyIndices[i];

			if (data.hydropathyIndices[i] < data.minScore)
				data.minScore = data.hydropathyIndices[i];
		}

		sumOfAll /= indexCount;
		std::cout << sumOfAll << '\n';
		return true;
	}
}