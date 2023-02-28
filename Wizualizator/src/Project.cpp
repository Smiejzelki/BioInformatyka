#include "Project.hpp"
#include "Elements.hpp"
#include "Transform.hpp"

constinit static std::unique_ptr<Project> s_Project{ nullptr };

void RnaMetadata::DeserializeFrame(RnaMetadata& outMetadata, const uint32_t index, const Bio::RnaSequence& rnaSequence)
{
	auto& frame{ outMetadata.Frames[index] };
	frame.RnaSequence = rnaSequence;
	frame.AminoSequence = Bio::TranslateNucleotideSequence(rnaSequence);

	auto& aminoSequence{ frame.AminoSequence };
	auto& proteinCandidates{ frame.ProteinCandidates };

	for (uint32_t i{ 0U }; i < aminoSequence.size(); ++i)
	{
		char currentAmino{ aminoSequence[i].AsCharacter() };

		/* Start */
		if (currentAmino == 'M')
		{
			Bio::AminoSequence candidate;

			do
			{
				candidate.emplace_back(Bio::AminoAcid{}.AssignCharacter(currentAmino));
				++i;
			} while (i < aminoSequence.size() && (currentAmino = aminoSequence[i].AsCharacter()) != '-');

			proteinCandidates.emplace_back(std::move(candidate));
		}
	}
}

void DnaMetadata::DeserializeFrame(DnaMetadata& outMetadata, const uint32_t frameIndex, const Bio::DnaSequence& rnaSequence)
{
	auto& frame{ outMetadata.Frames[frameIndex] };
	frame.DnaSequence = rnaSequence;
	frame.AminoSequence = Bio::TranslateNucleotideSequence(rnaSequence);

	auto& aminoSequence{ frame.AminoSequence };
	auto& proteinCandidates{ frame.ProteinCandidates };

	for (uint32_t i{ 0U }; i < aminoSequence.size(); ++i)
	{
		char currentAmino{ aminoSequence[i].AsCharacter() };

		/* Start */
		if (currentAmino == 'M')
		{
			Bio::AminoSequence candidate;

			do
			{
				candidate.emplace_back(Bio::AminoAcid{}.AssignCharacter(currentAmino));
				++i;
			} while (i < aminoSequence.size() && (currentAmino = aminoSequence[i].AsCharacter()) != '-');

			proteinCandidates.emplace_back(std::move(candidate));
		}
	}
}

void Project::InvalidateSelectionContext(const ESequenceSelectionType selectionType, const ID sequenceID, const ID frameIndex, const ID peptideID)
{
	if (s_SelectionContext.Type != selectionType || s_SelectionContext.Sequence != sequenceID || s_SelectionContext.Frame != frameIndex)
	{
		s_NucleotideSequenceCache.reset();
		s_NucleotideSequencePeptideCache.reset();
		s_AminoSequenceCache.reset();
		s_AminoSequencePeptideCache.reset();
	}
	else if (s_SelectionContext.Peptide != peptideID)
	{
		s_NucleotideSequencePeptideCache.reset();
		s_AminoSequencePeptideCache.reset();
	}

	s_SelectionContext.Type = selectionType;
	s_SelectionContext.Sequence = sequenceID;
	s_SelectionContext.Frame = frameIndex;
	s_SelectionContext.Peptide = peptideID;
#ifdef _DEBUG
	if (frameIndex != g_InvalidID)
		BIO_ASSERT(sequenceID != g_InvalidID);

	if (sequenceID == g_InvalidID)
	{
		BIO_ASSERT(frameIndex == g_InvalidID);
		BIO_ASSERT(peptideID == g_InvalidID);
	}

	if(selectionType != ESequenceSelectionType::AminoSequence)
	if (frameIndex == g_InvalidID)
	{
		BIO_ASSERT(sequenceID == g_InvalidID);
		BIO_ASSERT(peptideID == g_InvalidID);
	}
#endif

	for (const auto& callback : m_SelectionContextUpdateCallbacks)
		callback();
}

void Project::SetPathCallback(const std::function<std::filesystem::path()> callback) noexcept
{
	s_PathCallbackFunction = callback;
}

Project::~Project() noexcept
{}

std::unique_ptr<Project>& Project::Create()
{
	s_Project.release();
	s_Project = std::make_unique<Project>();

	return s_Project;
}

std::unique_ptr<Project>& Project::Get()
{
	BIO_ASSERT(s_Project);
	return s_Project;
}

ID Project::GenerateUUID() noexcept
{
	static constinit ID f_Counter{ 0U };
	return ++f_Counter;
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

Project::CalculationSettingsContext& Project::GetCalculationContext() noexcept
{
	return s_CalculationContext;
}

void Project::RecalculateHydropathy() noexcept
{
	if (Project::SelectedSequence())
	{
		switch (Project::SelectedSequenceType())
		{
			case ESequenceSelectionType::NucleotideSequence:
			{
				if (s_NucleotideSequenceCache.has_value())
				{
					{
						Bio::HydropathyPlotData plotData;
						if (Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(s_NucleotideSequenceCache->AminoSequence), s_CalculationContext.Hydropathy.WindowSize, plotData))
						{
							s_NucleotideSequenceCache->HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
								(
									HydropathyPlotData_t
									{
										.HydropathyIndices{ std::move(plotData.hydropathyIndices) },
										.MaxScore{ std::move(plotData.maxScore) },
										.MinScore{ std::move(plotData.minScore) }
									}
							);
						}
						else
							s_NucleotideSequenceCache->HydropathyPlotData.reset();
					}

					if (Project::SelectedPeptide() && s_NucleotideSequencePeptideCache.has_value())
					{
						Bio::HydropathyPlotData plotDataPeptide;
						if (Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(s_NucleotideSequencePeptideCache->ProteinCandidate), s_CalculationContext.Hydropathy.WindowSize, plotDataPeptide))
						{
							s_NucleotideSequencePeptideCache->HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
								(
									HydropathyPlotData_t
									{
										.HydropathyIndices{ std::move(plotDataPeptide.hydropathyIndices) },
										.MaxScore{ std::move(plotDataPeptide.maxScore) },
										.MinScore{ std::move(plotDataPeptide.minScore) }
									}
							);
						}
						else
							s_NucleotideSequencePeptideCache->HydropathyPlotData.reset();
					}
				}
			} break;

			case ESequenceSelectionType::AminoSequence:
			{
				if (s_AminoSequenceCache.has_value())
				{
					{
						Bio::HydropathyPlotData plotData;
						if (Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(s_AminoSequenceCache->AminoSequence), s_CalculationContext.Hydropathy.WindowSize, plotData))
						{
							s_AminoSequenceCache->HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
							(
								HydropathyPlotData_t
								{
									.HydropathyIndices{ std::move(plotData.hydropathyIndices) },
									.MaxScore{ std::move(plotData.maxScore) },
									.MinScore{ std::move(plotData.minScore) }
								}
							);
						}
						else
							s_AminoSequenceCache->HydropathyPlotData.reset();
					}

					if (Project::SelectedPeptide() && s_AminoSequencePeptideCache.has_value())
					{
						Bio::HydropathyPlotData plotDataPeptide;
						if (Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(s_AminoSequencePeptideCache->ProteinCandidate), s_CalculationContext.Hydropathy.WindowSize, plotDataPeptide))
						{
							s_AminoSequencePeptideCache->HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
							(
								HydropathyPlotData_t
								{
									.HydropathyIndices{ std::move(plotDataPeptide.hydropathyIndices) },
									.MaxScore{ std::move(plotDataPeptide.maxScore) },
									.MinScore{ std::move(plotDataPeptide.minScore) }
								}
							);
						}
						else
							s_AminoSequencePeptideCache->HydropathyPlotData.reset();
					}
				}
			} break;

			default:
				BIO_ASSERT(false);
				break;
		}
	}
}

void Project::RecalculateNetCharge() noexcept
{
	if (Project::SelectedSequence())
	{
		switch (Project::SelectedSequenceType())
		{
		case ESequenceSelectionType::NucleotideSequence:
		{
			if (s_NucleotideSequenceCache.has_value())
			{
				s_NucleotideSequenceCache->NetCharge = Bio::CalculateNetCharge(s_NucleotideSequenceCache->AminoSequence, s_CalculationContext.NetCharge.PH);

				if (Project::SelectedPeptide() && s_NucleotideSequencePeptideCache.has_value())
					s_NucleotideSequencePeptideCache->NetCharge = Bio::CalculateNetCharge(s_NucleotideSequencePeptideCache->ProteinCandidate, s_CalculationContext.NetCharge.PH);
			}
		} break;

		case ESequenceSelectionType::AminoSequence:
		{
			if (s_AminoSequenceCache.has_value())
			{
				s_AminoSequenceCache->NetCharge = Bio::CalculateNetCharge(s_AminoSequenceCache->AminoSequence, s_CalculationContext.NetCharge.PH);

				if (Project::SelectedPeptide() && s_AminoSequencePeptideCache.has_value())
					s_AminoSequencePeptideCache->NetCharge = Bio::CalculateNetCharge(s_AminoSequencePeptideCache->ProteinCandidate, s_CalculationContext.NetCharge.PH);
			}
		} break;

		default:
			BIO_ASSERT(false);
			break;
		}
	}
}

bool Project::OnSequenceSelected(
	const std::function<void(const NucleotideSequenceCache&)> onNucleotideSequenceSelected,
	const std::function<void(const NucleotideSequencePeptideCache&)> onNucleotideSequencePeptideSelected,
	const std::function<void(const AminoSequenceCache&)> onAminoSequenceSelected,
	const std::function<void(const AminoSequencePeptideCache&)> onAminoSequencePeptideSelected)
{
	RecalculateHydropathy();
	RecalculateNetCharge();
	if (Project::SelectedSequence())
	{
		switch (Project::SelectedSequenceType())
		{
			case ESequenceSelectionType::NucleotideSequence:
			{
				BIO_ASSERT(Project::SelectedFrame());
				if (!s_NucleotideSequenceCache.has_value())
				{
					NucleotideSequenceCache& nucleotideSequenceCache = s_NucleotideSequenceCache.emplace(NucleotideSequenceCache
					{
						.SequenceName{},
						.NucleotideSequence{},
						.AminoSequence{},
						.ProteinCandidates{},
						.ProteinCandidateLengths{},
						.MolecularWeight{},
						.IsoeletricPoint{},
						.NetCharge{},
					});

					const auto& sequence{ s_SequenceRegistry[Project::SelectedSequence()] };
					std::visit(overloaded
					{
						[&](const auto& sequenceMetadata)
						{
							nucleotideSequenceCache.SequenceName		= sequenceMetadata.GetName();
							nucleotideSequenceCache.NucleotideSequence	= sequenceMetadata.BakeSequence(Project::SelectedFrame());
							nucleotideSequenceCache.AminoSequence		= sequenceMetadata.BakeAminoSequence(Project::SelectedFrame());
							nucleotideSequenceCache.ProteinCandidates	= sequenceMetadata.BakeProteinCandidates(Project::SelectedFrame());

							nucleotideSequenceCache.ProteinCandidateLengths.resize(nucleotideSequenceCache.ProteinCandidates.size());
							for (size_t i{ 0U }; i < nucleotideSequenceCache.ProteinCandidates.size(); ++i)
								nucleotideSequenceCache.ProteinCandidateLengths[i] = static_cast<uint32_t>(nucleotideSequenceCache.ProteinCandidates[i].size());

							BIO_LIKELY
							if (!nucleotideSequenceCache.AminoSequence.empty())
							{
								const std::string_view sequenceView{ nucleotideSequenceCache.AminoSequence };
								nucleotideSequenceCache.MolecularWeight = Bio::CalculateMolecularWeight(sequenceView);
								nucleotideSequenceCache.IsoeletricPoint = Bio::CalculateIsoelectricPoint(sequenceView);
								nucleotideSequenceCache.NetCharge		= Bio::CalculateNetCharge(sequenceView, s_CalculationContext.NetCharge.PH);
								nucleotideSequenceCache.Formula			= Bio::GeneratePeptideFormula(sequenceView);

								auto isoelectricPlotData{ Bio::GenerateIsoelectricPlotData(sequenceView) };
								nucleotideSequenceCache.IsoeletricPointPlotData = std::make_shared<IsoelectricPointPlotData_t>
								(
									IsoelectricPointPlotData_t
									{
										.TestValues{ std::move(isoelectricPlotData.first) },
										.TestResults{ std::move(isoelectricPlotData.second) }
									}
								);

								if (nucleotideSequenceCache.AminoSequence.size() > s_CalculationContext.Hydropathy.WindowSize)
								{
									Bio::HydropathyPlotData plotData;
									Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(nucleotideSequenceCache.AminoSequence), s_CalculationContext.Hydropathy.WindowSize, plotData);

									nucleotideSequenceCache.HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
									(
										HydropathyPlotData_t
										{
											.HydropathyIndices{ std::move(plotData.hydropathyIndices) },
											.MaxScore{ std::move(plotData.maxScore) },
											.MinScore{ std::move(plotData.minScore) }
										}
									);
								}
								else
									nucleotideSequenceCache.HydropathyPlotData.reset();
							}
							else
							{
								nucleotideSequenceCache.MolecularWeight.reset();
								nucleotideSequenceCache.IsoeletricPoint.reset();
								nucleotideSequenceCache.NetCharge.reset();
								nucleotideSequenceCache.Formula.reset();
								nucleotideSequenceCache.IsoeletricPointPlotData.reset();
								nucleotideSequenceCache.HydropathyPlotData.reset();
							}
						}
					},
					sequence);
				}

				if (Project::SelectedPeptide() && !s_NucleotideSequencePeptideCache.has_value())
				{
					NucleotideSequencePeptideCache& nucleotideSequencePeptideCache = s_NucleotideSequencePeptideCache.emplace(NucleotideSequencePeptideCache
					{
						.ParentCache{ s_NucleotideSequenceCache.value() },
						.SequenceName{},
						.ProteinCandidate{},
						.PeptideIndex{ Project::SelectedPeptide() },
						.MolecularWeight{},
						.IsoeletricPoint{},
						.NetCharge{},
					});

					nucleotideSequencePeptideCache.SequenceName					= s_NucleotideSequenceCache->SequenceName;
					nucleotideSequencePeptideCache.ProteinCandidate				= s_NucleotideSequenceCache->ProteinCandidates[Project::SelectedPeptide()];
					nucleotideSequencePeptideCache.AminoSequenceThreeLetterCode = Bio::ConvertAminoSequenceToThreeLetterCode(Bio::ConvertToAminoSequence(nucleotideSequencePeptideCache.ProteinCandidate));
					nucleotideSequencePeptideCache.PeptideIndex					= Project::SelectedPeptide();

					BIO_LIKELY
					if (!nucleotideSequencePeptideCache.ProteinCandidate.empty())
					{
						const std::string_view sequenceView{ nucleotideSequencePeptideCache.ProteinCandidate };
						nucleotideSequencePeptideCache.MolecularWeight				= Bio::CalculateMolecularWeight(sequenceView);
						nucleotideSequencePeptideCache.IsoeletricPoint				= Bio::CalculateIsoelectricPoint(sequenceView);
						nucleotideSequencePeptideCache.NetCharge					= Bio::CalculateNetCharge(sequenceView, s_CalculationContext.NetCharge.PH);
						nucleotideSequencePeptideCache.ExtinctionCoefficient		= Bio::CalculateExtinctionCoefficient(sequenceView);
						nucleotideSequencePeptideCache.ExtinctionCoefficientReduced = Bio::CalculateExtinctionCoefficientCysteinesReduced(sequenceView);
						nucleotideSequencePeptideCache.Formula						= Bio::GeneratePeptideFormula(sequenceView);

						auto isoelectricPlotData{ Bio::GenerateIsoelectricPlotData(sequenceView) };
						nucleotideSequencePeptideCache.IsoeletricPointPlotData = std::make_shared<IsoelectricPointPlotData_t>
						(
							IsoelectricPointPlotData_t
							{
								.TestValues{ std::move(isoelectricPlotData.first) },
								.TestResults{ std::move(isoelectricPlotData.second) }
							}
						);

						if (nucleotideSequencePeptideCache.ProteinCandidate.size() > s_CalculationContext.Hydropathy.WindowSize)
						{
							Bio::HydropathyPlotData plotData;
							Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(nucleotideSequencePeptideCache.ProteinCandidate), s_CalculationContext.Hydropathy.WindowSize, plotData);

							nucleotideSequencePeptideCache.HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
							(
								HydropathyPlotData_t
								{
									.HydropathyIndices{ std::move(plotData.hydropathyIndices) },
									.MaxScore{ std::move(plotData.maxScore) },
									.MinScore{ std::move(plotData.minScore) }
								}
							);
						}
						else
							nucleotideSequencePeptideCache.HydropathyPlotData.reset();
					}
					else
					{
						nucleotideSequencePeptideCache.MolecularWeight.reset();
						nucleotideSequencePeptideCache.IsoeletricPoint.reset();
						nucleotideSequencePeptideCache.NetCharge.reset();
						nucleotideSequencePeptideCache.Formula.reset();
						nucleotideSequencePeptideCache.IsoeletricPointPlotData.reset();
						nucleotideSequencePeptideCache.HydropathyPlotData.reset();
					}
				}

				if (const auto peptideIndex = Project::SelectedPeptide())
				{
					if(onNucleotideSequencePeptideSelected)
						onNucleotideSequencePeptideSelected(s_NucleotideSequencePeptideCache.value());
				}
				else
				{
					if(onNucleotideSequenceSelected)
						onNucleotideSequenceSelected(s_NucleotideSequenceCache.value());
				}
			} break;

			case ESequenceSelectionType::AminoSequence:
			{
				if (!s_AminoSequenceCache.has_value())
				{
					AminoSequenceCache& aminoSequenceCache = s_AminoSequenceCache.emplace(AminoSequenceCache
					{
						.SequenceName{},
						.AminoSequence{},
						.AminoSequenceThreeLetterCode{},
						.ProteinCandidates{},
						.ProteinCandidateLengths{},
						.MolecularWeight{},
						.IsoeletricPoint{},
						.NetCharge{},
					});

					const auto& sequence{ s_SequenceRegistry[Project::SelectedSequence()] };
					std::visit(overloaded
					{
						[&](const auto& sequenceMetadata)
						{
							aminoSequenceCache.SequenceName		 = sequenceMetadata.GetName();
							aminoSequenceCache.AminoSequence	 = sequenceMetadata.BakeAminoSequence(Project::SelectedFrame());
							aminoSequenceCache.ProteinCandidates = sequenceMetadata.BakeProteinCandidates(Project::SelectedFrame());
							
							if constexpr (std::is_same_v<decltype(sequenceMetadata), const AminoMetadata&>)
								aminoSequenceCache.AminoSequenceThreeLetterCode = Bio::ConvertAminoSequenceToThreeLetterCode(sequenceMetadata.AminoSequence);

							aminoSequenceCache.ProteinCandidateLengths.resize(aminoSequenceCache.ProteinCandidates.size());
							for (size_t i{ 0U }; i < aminoSequenceCache.ProteinCandidates.size(); ++i)
								aminoSequenceCache.ProteinCandidateLengths[i] = static_cast<uint32_t>(aminoSequenceCache.ProteinCandidates[i].size());

							BIO_LIKELY
							if (!aminoSequenceCache.AminoSequence.empty())
							{
								const std::string_view sequenceView{ aminoSequenceCache.AminoSequence };
								aminoSequenceCache.MolecularWeight					= Bio::CalculateMolecularWeight(sequenceView);
								aminoSequenceCache.IsoeletricPoint					= Bio::CalculateIsoelectricPoint(sequenceView);
								aminoSequenceCache.NetCharge						= Bio::CalculateNetCharge(sequenceView, s_CalculationContext.NetCharge.PH);
								aminoSequenceCache.ExtinctionCoefficient			= Bio::CalculateExtinctionCoefficient(sequenceView);
								aminoSequenceCache.ExtinctionCoefficientReduced		= Bio::CalculateExtinctionCoefficientCysteinesReduced(sequenceView);
								aminoSequenceCache.Formula							= Bio::GeneratePeptideFormula(sequenceView);

								auto isoelectricPlotData{ Bio::GenerateIsoelectricPlotData(sequenceView) };
								aminoSequenceCache.IsoeletricPointPlotData = std::make_shared<IsoelectricPointPlotData_t>
								(
									IsoelectricPointPlotData_t
									{
										.TestValues{ std::move(isoelectricPlotData.first) },
										.TestResults{ std::move(isoelectricPlotData.second) }
									}
								);

								if (aminoSequenceCache.AminoSequence.size() > s_CalculationContext.Hydropathy.WindowSize)
								{
									Bio::HydropathyPlotData plotData;
									Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(aminoSequenceCache.AminoSequence), s_CalculationContext.Hydropathy.WindowSize, plotData);

									aminoSequenceCache.HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
									(
										HydropathyPlotData_t
										{
											.HydropathyIndices{ std::move(plotData.hydropathyIndices) },
											.MaxScore{ std::move(plotData.maxScore) },
											.MinScore{ std::move(plotData.minScore) }
										}
									);
								}
								else
									aminoSequenceCache.HydropathyPlotData.reset();
							}
							else
							{
								aminoSequenceCache.MolecularWeight.reset();
								aminoSequenceCache.IsoeletricPoint.reset();
								aminoSequenceCache.NetCharge.reset();
								aminoSequenceCache.Formula.reset();
								aminoSequenceCache.IsoeletricPointPlotData.reset();
								aminoSequenceCache.HydropathyPlotData.reset();
							}
						}
					},
					sequence);
				}

				if (Project::SelectedPeptide() && !s_AminoSequencePeptideCache.has_value())
				{
					AminoSequencePeptideCache& aminoSequencePeptideCache = s_AminoSequencePeptideCache.emplace(AminoSequencePeptideCache
					{
						.ParentCache{ s_AminoSequenceCache.value() },
						.SequenceName{},
						.ProteinCandidate{},
						.PeptideIndex{ Project::SelectedPeptide() },
						.MolecularWeight{},
						.IsoeletricPoint{},
						.NetCharge{},
					});

					aminoSequencePeptideCache.SequenceName					= s_AminoSequenceCache->SequenceName;
					aminoSequencePeptideCache.ProteinCandidate				= s_AminoSequenceCache->ProteinCandidates[Project::SelectedPeptide()];
					aminoSequencePeptideCache.PeptideIndex					= Project::SelectedPeptide();
					aminoSequencePeptideCache.AminoSequenceThreeLetterCode	= Bio::ConvertAminoSequenceToThreeLetterCode(Bio::ConvertToAminoSequence(aminoSequencePeptideCache.ProteinCandidate));

					BIO_LIKELY
					if (!aminoSequencePeptideCache.ProteinCandidate.empty())
					{
						const std::string_view sequenceView{ aminoSequencePeptideCache.ProteinCandidate };
						aminoSequencePeptideCache.MolecularWeight				= Bio::CalculateMolecularWeight(sequenceView);
						aminoSequencePeptideCache.IsoeletricPoint				= Bio::CalculateIsoelectricPoint(sequenceView);
						aminoSequencePeptideCache.NetCharge						= Bio::CalculateNetCharge(sequenceView, s_CalculationContext.NetCharge.PH);
						aminoSequencePeptideCache.ExtinctionCoefficient			= Bio::CalculateExtinctionCoefficient(sequenceView);
						aminoSequencePeptideCache.ExtinctionCoefficientReduced	= Bio::CalculateExtinctionCoefficientCysteinesReduced(sequenceView);
						aminoSequencePeptideCache.Formula						= Bio::GeneratePeptideFormula(sequenceView);

						auto isoelectricPlotData{ Bio::GenerateIsoelectricPlotData(sequenceView) };
						aminoSequencePeptideCache.IsoeletricPointPlotData = std::make_shared<IsoelectricPointPlotData_t>
						(
							IsoelectricPointPlotData_t
							{
								.TestValues{ std::move(isoelectricPlotData.first) },
								.TestResults{ std::move(isoelectricPlotData.second) }
							}
						);

						if (aminoSequencePeptideCache.ProteinCandidate.size() > s_CalculationContext.Hydropathy.WindowSize)
						{
							Bio::HydropathyPlotData plotData;
							Bio::GenerateHydropathyPlotData(Bio::ConvertToAminoSequence(aminoSequencePeptideCache.ProteinCandidate), s_CalculationContext.Hydropathy.WindowSize, plotData);

							aminoSequencePeptideCache.HydropathyPlotData = std::make_shared<HydropathyPlotData_t>
							(
								HydropathyPlotData_t
								{
									.HydropathyIndices{ std::move(plotData.hydropathyIndices) },
									.MaxScore{ std::move(plotData.maxScore) },
									.MinScore{ std::move(plotData.minScore) }
								}
							);
						}
						else
							aminoSequencePeptideCache.HydropathyPlotData.reset();
					}
					else
					{
						aminoSequencePeptideCache.MolecularWeight.reset();
						aminoSequencePeptideCache.IsoeletricPoint.reset();
						aminoSequencePeptideCache.NetCharge.reset();
						aminoSequencePeptideCache.Formula.reset();
						aminoSequencePeptideCache.IsoeletricPointPlotData.reset();
						aminoSequencePeptideCache.HydropathyPlotData.reset();
					}
				}

				if (const auto peptideIndex = Project::SelectedPeptide())
				{
					if(onAminoSequencePeptideSelected)
						onAminoSequencePeptideSelected(s_AminoSequencePeptideCache.value());
				}
				else
				{
					if(onAminoSequenceSelected)
						onAminoSequenceSelected(s_AminoSequenceCache.value());
				}
			} break;

		default:
			BIO_ASSERT(false);
			break;
		}
	}

	return Project::SelectedSequence() != g_InvalidID;
}

std::filesystem::path Project::GetWorkingPath()
{
	return s_PathCallbackFunction();
}

ProjectSerializer::ProjectSerializer(std::unique_ptr<Project>& project) noexcept
	:
	m_Project(project)
{
	BIO_ASSERT(m_Project);
}

std::ostream& operator<<(std::ostream& stream, const Bio::DnaSequence& sequence) noexcept
{
	for (const auto element : sequence)
		stream << element.AsCharacter();

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Bio::RnaSequence& sequence) noexcept
{
	for (const auto element : sequence)
		stream << element.AsCharacter();

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Bio::AminoSequence& sequence) noexcept
{
	for (const auto element : sequence)
		stream << element.AsCharacter();

	return stream;
}

void ProjectSerializer::OnSerialize(const std::filesystem::path& path) const
{
	BIO_UNLIKELY
	if (!m_Project)
		THROW_EXCEPTION("Saving invalid project");

	try
	{
		std::ofstream output(path, std::ios::binary);

		BIO_UNLIKELY
		if (!output.is_open())
			return THROW_EXCEPTION("Failed to open file");

		for (const auto& [sequenceUUID, metadata] : Project::Get()->s_SequenceRegistry)
		{
			std::visit(overloaded
			{
				[&](const DnaMetadata& dnaMetadata)
				{
					output << '#' << s_DNASequenceTypeToken << dnaMetadata.SequenceName << std::endl;

					for (size_t i{ 0U }; i < g_FrameCount; ++i)
					{
						output << dnaMetadata.Frames[i].DnaSequence << std::endl;
						output << dnaMetadata.Frames[i].AminoSequence << std::endl;

						/* TODO: Skip if empty? */
						output << s_BeginLoopToken << std::endl;
						for (const auto& candidate : dnaMetadata.Frames[i].ProteinCandidates)
							output << candidate << std::endl;

						output << s_EndLoopToken << std::endl;
					}
				},
			
				[&](const RnaMetadata& rnaMetadata)
				{
					output << '#' << s_RNASequenceTypeToken << rnaMetadata.SequenceName << std::endl;

					for (size_t i{ 0U }; i < g_FrameCount; ++i)
					{
						output << rnaMetadata.Frames[i].RnaSequence << std::endl;
						output << rnaMetadata.Frames[i].AminoSequence << std::endl;

						/* TODO: Skip if empty? */
						output << s_BeginLoopToken << std::endl;
						for (const auto& candidate : rnaMetadata.Frames[i].ProteinCandidates)
							output << candidate << std::endl;

						output << s_EndLoopToken << std::endl;
					}
				},
			
				[&](const AminoMetadata& aminoMetadata)
				{
					output << '#' << s_PeptideSequenceTypeToken << aminoMetadata.SequenceName << std::endl;

					output << aminoMetadata.AminoSequence << std::endl;

					/* TODO: Skip if empty? */
					output << s_BeginLoopToken << std::endl;
					for (const auto& candidate : aminoMetadata.ProteinCandidates)
						output << candidate << std::endl;

					output << s_EndLoopToken << std::endl;
				},
			
				[](const auto& arg) { BIO_ASSERT(false); (void)arg; }
			},
			metadata);
		}

		output.close();
	}
	catch (...)
	{
		HandleExceptions();
	}
}

void ProjectSerializer::OnDeserialize(const std::filesystem::path& path)
{
	BIO_UNLIKELY
	if (!m_Project)
		THROW_EXCEPTION("Deserializing into invalid project");

	BIO_LIKELY
	if (std::filesystem::exists(path))
	{
		try
		{
			std::ifstream input(path, std::ios::binary);
			
			BIO_UNLIKELY
			if (!input.is_open())
				THROW_EXCEPTION("Failed to open file");

			std::string currentLine;
			while (std::getline(input, currentLine))
			{
				if (currentLine.empty())
					THROW_EXCEPTION("Invalid file format");

				if (const std::size_t typeTokenPosition = currentLine.find(s_SequenceTypeToken) != std::string::npos)
				{
					static_assert(
						sizeof(s_DNASequenceTypeToken) == 
						sizeof(s_RNASequenceTypeToken) &&
						sizeof(s_RNASequenceTypeToken) ==
						sizeof(s_PeptideSequenceTypeToken));

					const std::string metadataTypeString{ currentLine.substr(typeTokenPosition, 4U) };
					const std::string sequenceName{ currentLine.substr(currentLine.find(':') + 1U) };

					if (metadataTypeString == s_DNASequenceTypeToken)
					{
						auto& ref{ Project::RegisterSequence<DnaMetadata>("")};
						DnaMetadata deserialized{ sequenceName };

						for (uint32_t i{ 0U }; i < 3U; ++i)
						{
							std::getline(input, currentLine);
							deserialized.Frames[i].DnaSequence = Bio::ConvertToDNA(currentLine);

							std::getline(input, currentLine);
							deserialized.Frames[i].AminoSequence = Bio::ConvertToAminoSequence(currentLine);

							std::vector<Bio::AminoSequence> proteinCandidates;
							if (std::getline(input, currentLine) && currentLine.find(s_BeginLoopToken) != std::string::npos)
							{
								std::getline(input, currentLine);
								while (currentLine.find(s_EndLoopToken) == std::string::npos)
								{
									proteinCandidates.emplace_back(Bio::ConvertToAminoSequence(currentLine));
									std::getline(input, currentLine);
								}
							}

							deserialized.Frames[i].ProteinCandidates = std::move(proteinCandidates);
						}

						ref = std::move(deserialized);
					}
					else if (metadataTypeString == s_RNASequenceTypeToken)
					{
						auto& ref{ Project::RegisterSequence<RnaMetadata>("") };
						RnaMetadata deserialized{ sequenceName };

						for (uint32_t i{ 0U }; i < g_FrameCount; ++i)
						{
							std::getline(input, currentLine);
							deserialized.Frames[i].RnaSequence = Bio::ConvertToRNA(currentLine);

							std::getline(input, currentLine);
							deserialized.Frames[i].AminoSequence = Bio::ConvertToAminoSequence(currentLine);

							std::vector<Bio::AminoSequence> proteinCandidates;
							if (std::getline(input, currentLine) && currentLine.find(s_BeginLoopToken) != std::string::npos)
							{
								std::getline(input, currentLine);
								while (currentLine.find(s_EndLoopToken) == std::string::npos)
								{
									proteinCandidates.emplace_back(Bio::ConvertToAminoSequence(currentLine));
									std::getline(input, currentLine);
								}
							}

							deserialized.Frames[i].ProteinCandidates = std::move(proteinCandidates);
						}

						ref = std::move(deserialized);
					}
					else if (metadataTypeString == s_PeptideSequenceTypeToken)
					{
						auto& ref{ Project::RegisterSequence<AminoMetadata>("") };
						AminoMetadata deserialized{ sequenceName };

						std::getline(input, currentLine);
						deserialized.AminoSequence = Bio::ConvertToAminoSequence(currentLine);

						std::vector<Bio::AminoSequence> proteinCandidates;
						if (std::getline(input, currentLine) && currentLine.find(s_BeginLoopToken) != std::string::npos)
						{
							std::getline(input, currentLine);
							while (currentLine.find(s_EndLoopToken) == std::string::npos)
							{
								proteinCandidates.emplace_back(Bio::ConvertToAminoSequence(currentLine));
								std::getline(input, currentLine);
							}
						}

						deserialized.ProteinCandidates = std::move(proteinCandidates);
						ref = std::move(deserialized);
					}
				}
			}
		}
		catch (...)
		{
			HandleExceptions();
		}
	}
	else
		fprintf(stderr, "Failed saving project (path doesn't exists)\n");
}