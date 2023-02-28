#pragma once
#include "Elements.hpp"
#include <stdint.h>
#include <string_view>
#include <vector>

namespace Bio {
	/*
	* Average mass
	*/
	double GetAminoMolecularWeight(const AminoAcid aminoAcid) noexcept
	{
		switch (aminoAcid)
		{
			case EAminoAcid::STOP:	return 0.0; // ?
			case EAminoAcid::V:		return 117.14634;
			case EAminoAcid::A:		return 89.09318;
			case EAminoAcid::D:		return 133.10268;
			case EAminoAcid::E:		return 147.12926;
			case EAminoAcid::G:		return 75.0666;
			case EAminoAcid::F:		return 165.18914;
			case EAminoAcid::L:		return 131.17292;
			case EAminoAcid::S:		return 105.09258;
			case EAminoAcid::Y:		return 181.18854;
			case EAminoAcid::C:		return 121.15818;
			case EAminoAcid::W:		return 204.22518;
			case EAminoAcid::P:		return 115.13046;
			case EAminoAcid::H:		return 155.15456;
			case EAminoAcid::Q:		return 146.1445;
			case EAminoAcid::R:		return 174.20096;
			case EAminoAcid::I:		return 131.17292;
			case EAminoAcid::M:		return 149.21134;
			case EAminoAcid::T:		return 119.11916;
			case EAminoAcid::N:		return 132.11792;
			case EAminoAcid::K:		return 146.18756;

			default:
				assert(false);
				break;
		}

		assert(false);
		return 0.0f;
	}

	double GetAminoMolecularWeight(const char aminoAcid)
	{
		switch (aminoAcid)
		{
			case '-': return 0.0; // ?
			case 'V': return 117.14634;
			case 'A': return 89.09318;
			case 'D': return 133.10268;
			case 'E': return 147.12926;
			case 'G': return 75.0666;
			case 'F': return 165.18914;
			case 'L': return 131.17292;
			case 'S': return 105.09258;
			case 'Y': return 181.18854;
			case 'C': return 121.15818;
			case 'W': return 204.22518;
			case 'P': return 115.13046;
			case 'H': return 155.15456;
			case 'Q': return 146.1445;
			case 'R': return 174.20096;
			case 'I': return 131.17292;
			case 'M': return 149.21134;
			case 'T': return 119.11916;
			case 'N': return 132.11792;
			case 'K': return 146.18756;
			
			default:
				assert(false);
				break;
		}
	
		assert(false);
		return 0.0f;
	}

	double CalculateMolecularWeight(const AminoSequence& sequence)
	{
		double molecularWeight{ 0.0 };
		for (const AminoAcid amino : sequence)
			molecularWeight += GetAminoMolecularWeight(amino);

		const double itsJustWaterWeight{ 18.01528 * (sequence.size() - 1U) };
		return molecularWeight - itsJustWaterWeight;
	}
	
	double CalculateMolecularWeight(const std::string_view sequence)
	{
		double molecularWeight{ 0.0 };
		for (const char amino : sequence)
			molecularWeight += GetAminoMolecularWeight(amino);

		const double itsJustWaterWeight{ 18.01528 * (sequence.length() - 1) };
		return molecularWeight - itsJustWaterWeight;
	}

	double CalculateNetCharge(const AminoSequence& sequence, const double pH)
	{
		if (sequence.empty())
			return 0.0f;

		double netCharge{ 0.0 };
		/* N terminus */
		{
			constexpr double chargeMagnitude{ 1.0 };
			double pKa{ 3.3 };
			switch (sequence.front())
			{
				case EAminoAcid::A: pKa = { 9.60 }; break;
				case EAminoAcid::R: pKa = { 9.04 }; break;
				case EAminoAcid::N: pKa = { 8.80 }; break;
				case EAminoAcid::D: pKa = { 9.60 }; break;
				case EAminoAcid::C: pKa = { 10.28 }; break;
				case EAminoAcid::Q: pKa = { 9.13 }; break;
				case EAminoAcid::E: pKa = { 9.67 }; break;
				case EAminoAcid::G: pKa = { 9.60 }; break;
				case EAminoAcid::H: pKa = { 9.17 }; break;
				case EAminoAcid::I: pKa = { 9.68 }; break;
				case EAminoAcid::L: pKa = { 9.60 }; break;
				case EAminoAcid::K: pKa = { 8.95 }; break;
				case EAminoAcid::M: pKa = { 9.21 }; break;
				case EAminoAcid::F: pKa = { 9.13 }; break;
				case EAminoAcid::P: pKa = { 9.68 }; break;
				case EAminoAcid::S: pKa = { 9.15 }; break;
				case EAminoAcid::T: pKa = { 9.62 }; break;
				case EAminoAcid::W: pKa = { 9.38 }; break;
				case EAminoAcid::Y: pKa = { 9.11 }; break;
				case EAminoAcid::V: pKa = { 9.62 }; break;

				default:
					break;
			}

			double result{ 0.0 };
			if (pH < pKa)
				result = chargeMagnitude;
			else if (pH == pKa)
				result = (chargeMagnitude * 0.5);

			netCharge += result;
		}

		/* C terminus */
		{
			constexpr double chargeMagnitude{ 1.0 };
			double pKa{ 3.3 };
			switch (sequence.back())
			{
				case EAminoAcid::A: pKa = { 2.34 }; break;
				case EAminoAcid::R: pKa = { 2.17 }; break;
				case EAminoAcid::N: pKa = { 2.02 }; break;
				case EAminoAcid::D: pKa = { 1.88 }; break;
				case EAminoAcid::C: pKa = { 1.96 }; break;
				case EAminoAcid::Q: pKa = { 2.17 }; break;
				case EAminoAcid::E: pKa = { 2.19 }; break;
				case EAminoAcid::G: pKa = { 2.34 }; break;
				case EAminoAcid::H: pKa = { 1.82 }; break;
				case EAminoAcid::I: pKa = { 2.36 }; break;
				case EAminoAcid::L: pKa = { 2.36 }; break;
				case EAminoAcid::K: pKa = { 2.18 }; break;
				case EAminoAcid::M: pKa = { 2.28 }; break;
				case EAminoAcid::F: pKa = { 1.83 }; break;
				case EAminoAcid::P: pKa = { 1.99 }; break;
				case EAminoAcid::S: pKa = { 2.21 }; break;
				case EAminoAcid::T: pKa = { 2.11 }; break;
				case EAminoAcid::W: pKa = { 2.38 }; break;
				case EAminoAcid::Y: pKa = { 2.20 }; break;
				case EAminoAcid::V: pKa = { 2.32 }; break;
				default:
					break;
			}

			double result{ 0.0 };
			if (pH > pKa)
				result = (-1.0 * chargeMagnitude);
			else if (pH == pKa)
				result = -1.0 * (chargeMagnitude * 0.5);

			netCharge += result;
		}

		for (const AminoAcid amino : sequence)
		{
			switch (amino)
			{
				case EAminoAcid::D:
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 3.65 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case EAminoAcid::E:
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 4.25 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case EAminoAcid::C:
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 8.18 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case EAminoAcid::Y:
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 10.07 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case EAminoAcid::K:
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 10.53 };

					double result{ 0.0 };
					if (pH < pKa)
						result = chargeMagnitude;
					else if (pH == pKa)
						result = (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case EAminoAcid::R:
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 12.48 };

					double result{ 0.0 };
					if (pH < pKa)
						result = chargeMagnitude;
					else if (pH == pKa)
						result = (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case EAminoAcid::H:
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 6.0 };

					double result{ 0.0 };
					if (pH < pKa)
						result = chargeMagnitude;
					else if (pH == pKa)
						result = (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				default:
					break;
			}
		}

		return netCharge;
	}

	double CalculateNetCharge(const std::string_view sequence, const double pH)
	{
		if (sequence.empty())
			return 0.0f;

		double netCharge{ 0.0 };

		std::vector<char> acidicSideChains{ 'D', 'E', 'C', 'Y' };
		std::vector<char> basicSideChains{ 'K', 'R', 'H' };

		/* N terminus */
		{
			constexpr double chargeMagnitude{ 1.0 };
			double pKa{ 3.3 };
			switch (sequence.front())
			{
				case 'A': pKa = { 9.60 }; break;
				case 'R': pKa = { 9.04 }; break;
				case 'N': pKa = { 8.80 }; break;
				case 'D': pKa = { 9.60 }; break;
				case 'C': pKa = { 10.28 }; break;
				case 'Q': pKa = { 9.13 }; break;
				case 'E': pKa = { 9.67 }; break;
				case 'G': pKa = { 9.60 }; break;
				case 'H': pKa = { 9.17 }; break;
				case 'I': pKa = { 9.68 }; break;
				case 'L': pKa = { 9.60 }; break;
				case 'K': pKa = { 8.95 }; break;
				case 'M': pKa = { 9.21 }; break;
				case 'F': pKa = { 9.13 }; break;
				case 'P': pKa = { 9.68 }; break;
				case 'S': pKa = { 9.15 }; break;
				case 'T': pKa = { 9.62 }; break;
				case 'W': pKa = { 9.38 }; break;
				case 'Y': pKa = { 9.11 }; break;
				case 'V': pKa = { 9.62 }; break;

				default:
					break;
			}

			double result{ 0.0 };
			if (pH < pKa)
				result = chargeMagnitude;
			else if (pH == pKa)
				result = (chargeMagnitude * 0.5);

			netCharge += result;
		}

		/* C terminus */
		{
			constexpr double chargeMagnitude{ 1.0 };
			double pKa{ 3.3 };
			switch (sequence.back())
			{
				case 'A': pKa = { 2.34 }; break;
				case 'R': pKa = { 2.17 }; break;
				case 'N': pKa = { 2.02 }; break;
				case 'D': pKa = { 1.88 }; break;
				case 'C': pKa = { 1.96 }; break;
				case 'Q': pKa = { 2.17 }; break;
				case 'E': pKa = { 2.19 }; break;
				case 'G': pKa = { 2.34 }; break;
				case 'H': pKa = { 1.82 }; break;
				case 'I': pKa = { 2.36 }; break;
				case 'L': pKa = { 2.36 }; break;
				case 'K': pKa = { 2.18 }; break;
				case 'M': pKa = { 2.28 }; break;
				case 'F': pKa = { 1.83 }; break;
				case 'P': pKa = { 1.99 }; break;
				case 'S': pKa = { 2.21 }; break;
				case 'T': pKa = { 2.11 }; break;
				case 'W': pKa = { 2.38 }; break;
				case 'Y': pKa = { 2.20 }; break;
				case 'V': pKa = { 2.32 }; break;
			default:
				break;
			}

			double result{ 0.0 };
			if (pH > pKa)
				result = (-1.0 * chargeMagnitude);
			else if (pH == pKa)
				result = -1.0 * (chargeMagnitude * 0.5);

			netCharge += result;
		}

		for (const char amino : sequence)
		{
			switch (amino)
			{
				case 'D':
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 3.65 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case 'E':
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 4.25 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case 'C':
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 8.18 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case 'Y':
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 10.07 };

					double result{ 0.0 };
					if (pH > pKa)
						result = (-1.0 * chargeMagnitude);
					else if (pH == pKa)
						result = -1.0 * (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case 'K':
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 10.53 };

					double result{ 0.0 };
					if (pH < pKa)
						result = chargeMagnitude;
					else if (pH == pKa)
						result = (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case 'R':
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 12.48 };

					double result{ 0.0 };
					if (pH < pKa)
						result = chargeMagnitude;
					else if (pH == pKa)
						result = (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				case 'H':
				{
					constexpr double chargeMagnitude{ 1.0 };
					constexpr double pKa{ 6.0 };

					double result{ 0.0 };
					if (pH < pKa)
						result = chargeMagnitude;
					else if (pH == pKa)
						result = (chargeMagnitude * 0.5);

					netCharge += result;
					break;
				}

				default:
					break;
			}
		}

		return netCharge;
	}

	double CalculateIsoelectricPoint(const AminoSequence& sequence) noexcept
	{
		if (sequence.empty())
			return 0.0;

		int64_t asparigineCount{ 0U };
		int64_t glutamicAcidCount{ 0U };
		int64_t cysteineCount{ 0U };
		int64_t tyrosineCount{ 0U };
		int64_t histidineCount{ 0U };
		int64_t lysineCount{ 0U };
		int64_t arginineCount{ 0U };

		for (const AminoAcid aminoAcid : sequence)
		{
			switch (aminoAcid)
			{
				case EAminoAcid::D: ++asparigineCount;		break;
				case EAminoAcid::E: ++glutamicAcidCount;	break;
				case EAminoAcid::C: ++cysteineCount;		break;
				case EAminoAcid::Y: ++tyrosineCount;		break;
				case EAminoAcid::H: ++histidineCount;		break;
				case EAminoAcid::K: ++lysineCount;			break;
				case EAminoAcid::R: ++arginineCount;		break;
				
				default: break;
			}
		}

		double cTerminalCharge{ 0.0 };
		double N2HTerminalCharge{ 0.0 };
		double asparticAcidCharge{ 0.0 };
		double glutamicAcidCharge{ 0.0 };
		double cysteineCharge{ 0.0 };
		double tyrosineCharge{ 0.0 };
		double histidineCharge{ 0.0 };
		double lysineCharge{ 0.0 };
		double arginineCharge{ 0.0 };

		double netCharge{ 0.0 };
		double pH{ 0.0 };
		do
		{
			cTerminalCharge			= -1					/ (1.0 + pow(10, (3.65 - pH)));
			N2HTerminalCharge		= 1						/ (1.0 + pow(10, (pH - 8.2)));
			asparticAcidCharge		= -asparigineCount		/ (1.0 + pow(10, (3.9 - pH)));
			glutamicAcidCharge		= -glutamicAcidCount	/ (1.0 + pow(10, (4.07 - pH)));
			cysteineCharge			= -cysteineCount		/ (1.0 + pow(10, (8.18 - pH)));
			tyrosineCharge			= -tyrosineCount		/ (1.0 + pow(10, (10.46 - pH)));
			histidineCharge			= histidineCount		/ (1.0 + pow(10, (pH - 6.04)));
			lysineCharge			= lysineCount			/ (1.0 + pow(10, (pH - 10.54)));
			arginineCharge			= arginineCount			/ (1.0 + pow(10, (pH - 12.48)));

			netCharge =
				+ cTerminalCharge
				+ N2HTerminalCharge
				+ asparticAcidCharge
				+ glutamicAcidCharge
				+ cysteineCharge
				+ tyrosineCharge
				+ histidineCharge
				+ lysineCharge
				+ arginineCharge;

			if (netCharge <= 0)
				break;

			if (pH >= 14.0)
				return 0.0; /* Should never happen */

			pH += 0.01;
		} while (true);

		return pH;
	}
	
	double CalculateIsoelectricPoint(const std::string_view protein) noexcept
	{
		if (protein.empty())
			return 0.0;

		int64_t asparigineCount{ 0U };
		int64_t glutamicAcidCount{ 0U };
		int64_t cysteineCount{ 0U };
		int64_t tyrosineCount{ 0U };
		int64_t histidineCount{ 0U };
		int64_t lysineCount{ 0U };
		int64_t arginineCount{ 0U };

		for (const char aminoAcid : protein)
		{
			switch (aminoAcid)
			{
				case 'D': ++asparigineCount; break;
				case 'E': ++glutamicAcidCount; break;
				case 'C': ++cysteineCount; break;
				case 'Y': ++tyrosineCount; break;
				case 'H': ++histidineCount; break;
				case 'K': ++lysineCount; break;
				case 'R': ++arginineCount; break;

				default: break;
			}
		}

		double cTerminalCharge{ 0.0 };
		double N2HTerminalCharge{ 0.0 };
		double asparticAcidCharge{ 0.0 };
		double glutamicAcidCharge{ 0.0 };
		double cysteineCharge{ 0.0 };
		double tyrosineCharge{ 0.0 };
		double histidineCharge{ 0.0 };
		double lysineCharge{ 0.0 };
		double arginineCharge{ 0.0 };

		double netCharge{ 0.0 };
		double pH{ 0.0 };
		do
		{
			cTerminalCharge		= -1 / (1 + pow(10, (3.65 - pH)));
			N2HTerminalCharge	= 1 / (1 + pow(10, (pH - 8.2)));
			asparticAcidCharge	= -asparigineCount / (1 + pow(10, (3.9 - pH)));
			glutamicAcidCharge	= -glutamicAcidCount / (1 + pow(10, (4.07 - pH)));
			cysteineCharge		= -cysteineCount / (1 + pow(10, (8.18 - pH)));
			tyrosineCharge		= -tyrosineCount / (1 + pow(10, (10.46 - pH)));
			histidineCharge		= histidineCount / (1 + pow(10, (pH - 6.04)));
			lysineCharge		= lysineCount / (1 + pow(10, (pH - 10.54)));
			arginineCharge		= arginineCount / (1 + pow(10, (pH - 12.48)));

			netCharge =
				+cTerminalCharge
				+ N2HTerminalCharge
				+ asparticAcidCharge
				+ glutamicAcidCharge
				+ cysteineCharge
				+ tyrosineCharge
				+ histidineCharge
				+ lysineCharge
				+ arginineCharge;

			if (netCharge <= 0)
				break;

			if (pH >= 14.0)
				return 0.0; /* Should never happen */

			pH += 0.01;
		} while (true);

		return pH;
	}

	std::pair<std::vector<double>, std::vector<double>> GenerateIsoelectricPlotData(const std::string_view protein)
	{
		if (protein.empty())
			return {};

		int64_t asparigineCount{ 0U };
		int64_t glutamicAcidCount{ 0U };
		int64_t cysteineCount{ 0U };
		int64_t tyrosineCount{ 0U };
		int64_t histidineCount{ 0U };
		int64_t lysineCount{ 0U };
		int64_t arginineCount{ 0U };

		for (const char aminoAcid : protein)
		{
			switch (aminoAcid)
			{
				case 'D': ++asparigineCount; break;
				case 'E': ++glutamicAcidCount; break;
				case 'C': ++cysteineCount; break;
				case 'Y': ++tyrosineCount; break;
				case 'H': ++histidineCount; break;
				case 'K': ++lysineCount; break;
				case 'R': ++arginineCount; break;

				default: break;
			}
		}

		double cTerminalCharge{ 0.0 };
		double N2HTerminalCharge{ 0.0 };
		double asparticAcidCharge{ 0.0 };
		double glutamicAcidCharge{ 0.0 };
		double cysteineCharge{ 0.0 };
		double tyrosineCharge{ 0.0 };
		double histidineCharge{ 0.0 };
		double lysineCharge{ 0.0 };
		double arginineCharge{ 0.0 };

		double netCharge{ 0.0 };
		
		std::vector<double> testedPHs;
		testedPHs.reserve(static_cast<size_t>(14.0 / 0.01));
		std::vector<double> results;
		results.reserve(static_cast<size_t>(14.0 / 0.01));

		for(double pH{ 0.0 }; pH < 14.0; pH += 0.01)
		{
			cTerminalCharge		= -1 / (1 + pow(10, (3.65 - pH)));
			N2HTerminalCharge	= 1 / (1 + pow(10, (pH - 8.2)));
			asparticAcidCharge	= -asparigineCount / (1 + pow(10, (3.9 - pH)));
			glutamicAcidCharge	= -glutamicAcidCount / (1 + pow(10, (4.07 - pH)));
			cysteineCharge		= -cysteineCount / (1 + pow(10, (8.18 - pH)));
			tyrosineCharge		= -tyrosineCount / (1 + pow(10, (10.46 - pH)));
			histidineCharge		= histidineCount / (1 + pow(10, (pH - 6.04)));
			lysineCharge		= lysineCount / (1 + pow(10, (pH - 10.54)));
			arginineCharge		= arginineCount / (1 + pow(10, (pH - 12.48)));

			netCharge =
				+cTerminalCharge
				+ N2HTerminalCharge
				+ asparticAcidCharge
				+ glutamicAcidCharge
				+ cysteineCharge
				+ tyrosineCharge
				+ histidineCharge
				+ lysineCharge
				+ arginineCharge;

			testedPHs.emplace_back(pH);
			results.emplace_back(netCharge);
		}

		return { testedPHs, results };
	}

	PeptideFormula GeneratePeptideFormula(const std::string_view sequence) noexcept
	{
		using namespace Bio::literals::elements;
		/* Starting formula */
		PeptideFormula formula{ 0_C, 2_H, 0_N, 1_O, 0_S };

		for (const char amino : sequence)
			formula += Bio::GetAminoAcidFormula(amino);

		return formula;
	}

	size_t CalculateExtinctionCoefficient(const AminoSequence& sequence) noexcept
	{
		size_t sum{ 0U };
		size_t cysteineCount{ 0U };
		for (const AminoAcid amino : sequence)
		{
			if (amino == EAminoAcid::C)
				++cysteineCount;

			if (amino == EAminoAcid::Y)
				sum += 1490U;

			if (amino == EAminoAcid::W)
				sum += 5500U;
		}

		/* Add cysteine pairs */
		return sum + (cysteineCount / 2U) * 125U;
	}

	size_t CalculateExtinctionCoefficient(const std::string_view sequence) noexcept
	{
		size_t sum{ 0U };
		size_t cysteineCount{ 0U };
		for (const char amino : sequence)
		{
			if (amino == 'C')
				++cysteineCount;

			if (amino == 'Y')
				sum += 1490U;

			if (amino == 'W')
				sum += 5500U;
		}

		/* Add cysteine pairs */
		return sum + (cysteineCount / 2U) * 125U;
	}

	size_t CalculateExtinctionCoefficientCysteinesReduced(const AminoSequence& sequence) noexcept
	{
		size_t sum{ 0U };
		for (const AminoAcid amino : sequence)
		{
			if (amino == EAminoAcid::Y)
				sum += 1490U;

			if (amino == EAminoAcid::W)
				sum += 5500U;
		}

		return sum;
	}

	size_t CalculateExtinctionCoefficientCysteinesReduced(const std::string_view sequence) noexcept
	{
		size_t sum{ 0U };
		for (const char amino : sequence)
		{
			if (amino == 'Y')
				sum += 1490U;

			if (amino == 'W')
				sum += 5500U;
		}

		return sum;
	}
}