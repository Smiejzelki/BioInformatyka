#pragma once
#include "Core.hpp"
#include "ID.hpp"
#include "Nucleotides.hpp"
#include "Elements.hpp"
#include "Hydropathy.hpp"

constexpr size_t g_FrameCount{ 3U };

template<typename Derived>
struct SequenceMetadata
{
	const std::string& GetName() const
	{
		(static_cast<Derived*>(this))->GetName();
	}

	const std::string BakeSequence(const EFrame frame) const
	{
		(static_cast<Derived*>(this))->BakeSequence(frame);
	}

	const std::string BakeAminoSequence(const EFrame frame) const
	{
		(static_cast<Derived*>(this))->BakeAminoSequence(frame);
	}

	const std::vector<std::string> BakeProteinCandidates(const EFrame frame) const
	{
		(static_cast<Derived*>(this))->BakeProteinCandidates(frame);
	}
private:
	friend Derived;
};

struct RnaMetadata final : public SequenceMetadata<RnaMetadata>
{
	std::string SequenceName;
	RnaMetadata() = default;

	RnaMetadata(const std::string& sequenceName)
		:
		SequenceName(sequenceName)
	{}

	const std::string& GetName() const
	{
		return SequenceName;
	}

	struct
	{
		Bio::RnaSequence RnaSequence;
		Bio::AminoSequence AminoSequence;
		std::vector<Bio::AminoSequence> ProteinCandidates;
	} Frames[g_FrameCount];

	static void DeserializeFrame(RnaMetadata& _this, const uint32_t index, const Bio::RnaSequence& nucleotide);

	static inline RnaMetadata Create(const std::string& sequenceName, const std::string_view sequence)
	{
		RnaMetadata metaData{ sequenceName };

		if (sequence.empty())
			return metaData;
		
		/* Deserialize Frame 1 (0 offset) */
		DeserializeFrame(metaData, 0, Bio::ConvertToRNA(sequence));

		/* Deserialize Frame 1 (+1 offset) */
		/* AUC GUU -> UCG UUA */
		DeserializeFrame(metaData, 1, Bio::ConvertToRNA(sequence.substr(1U)));

		/* Deserialize Frame 1 (+2 offset) */
		/* UCG UUA -> CGU UAU */
		DeserializeFrame(metaData, 2, Bio::ConvertToRNA(sequence.substr(2U)));
		return metaData;
	}
	
	const auto& GetFrame(const EFrame frame) const
	{
		return Frames[static_cast<size_t>(frame)];
	}

	const std::string BakeSequence(const EFrame frame) const
	{
		const auto& rnaSequence{ GetFrame(frame).RnaSequence };
		const size_t rnaSequenceSize{ rnaSequence.size() };

		std::string result;
		result.resize(rnaSequenceSize);
		for (size_t i{ 0U }; i < rnaSequenceSize; ++i)
			result[i] = rnaSequence[i].AsCharacter();

		return result;
	}

	const std::string BakeAminoSequence(const EFrame frame) const
	{
		const auto& aminoSequence{ GetFrame(frame).AminoSequence };
		const size_t aminoSequenceSize{ aminoSequence.size() };

		std::string result;
		result.resize(aminoSequenceSize);
		for (size_t i{ 0U }; i < aminoSequenceSize; ++i)
			result[i] = aminoSequence[i].AsCharacter();

		return result;
	}

	const std::vector<std::string> BakeProteinCandidates(const EFrame frame) const
	{
		const auto& proteinCandidates{ GetFrame(frame).ProteinCandidates };

		std::vector<std::string> results;
		results.reserve(proteinCandidates.size());
		for (const auto& proteinCandidate : proteinCandidates)
		{
			const size_t proteinSize{ proteinCandidate.size() };
			std::string result{};
			result.resize(proteinSize);

			for (size_t i{ 0U }; i < proteinSize; ++i)
				result[i] = proteinCandidate[i].AsCharacter();

			results.emplace_back(std::move(result));
		}

		return results;
	}
private:
};

struct DnaMetadata final : public SequenceMetadata<DnaMetadata>
{
	std::string SequenceName;
	struct
	{
		Bio::DnaSequence DnaSequence;
		Bio::AminoSequence AminoSequence;
		std::vector<Bio::AminoSequence> ProteinCandidates;
	} Frames[g_FrameCount];

	const auto& GetFrame(const EFrame frame) const
	{
		return Frames[static_cast<size_t>(frame)];
	}

	const std::string& GetName() const
	{
		return SequenceName;
	}

	DnaMetadata() = default;
	DnaMetadata(const std::string& sequenceName)
		:
		SequenceName(sequenceName)
	{}

	static void DeserializeFrame(DnaMetadata& _this, const uint32_t frameIndex, const Bio::DnaSequence& nucleotide);

	static inline DnaMetadata Create(const std::string& sequenceName, const std::string_view sequence, const bool reverse = false)
	{
		DnaMetadata metaData{ sequenceName };

		if (sequence.empty())
			return metaData;

		/* Deserialize Frame 1 (0 offset) */
		DeserializeFrame(metaData, 0U, reverse ? Bio::ConvertToDNAReversed(sequence) : Bio::ConvertToDNA(sequence));

		/* Deserialize Frame 1 (+1 offset) */
		/* AUC GUU -> UCG UUA */
		DeserializeFrame(metaData, 1U, 
			reverse ? 
			Bio::ConvertToDNAReversed(sequence.substr(0U, sequence.size() - 1U)) :
			Bio::ConvertToDNA(sequence.substr(1U)));

		/* Deserialize Frame 1 (+2 offset) */
		/* UCG UUA -> CGU UAU */
		const std::string_view secondSequenceShift{ sequence.substr(2U) };
		DeserializeFrame(metaData, 2U, 
			reverse ? 
			Bio::ConvertToDNAReversed(sequence.substr(0U, sequence.size() - 2U)) : 
			Bio::ConvertToDNA(sequence.substr(2U)));
		return metaData;
	}

	const std::string BakeSequence(const EFrame frame) const
	{
		const auto& dnaSequence{ GetFrame(frame).DnaSequence };
		const size_t dnaSequenceSize{ dnaSequence.size() };

		std::string result;
		result.resize(dnaSequenceSize);
		for (size_t i{ 0U }; i < dnaSequenceSize; ++i)
			result[i] = dnaSequence[i].AsCharacter();

		return result;
	}

	const std::string BakeAminoSequence(const EFrame frame) const
	{
		const auto& aminoSequence{ GetFrame(frame).AminoSequence };
		const size_t aminoSequenceSize{ aminoSequence.size() };

		std::string result;
		result.resize(aminoSequenceSize);
		for (size_t i{ 0U }; i < aminoSequenceSize; ++i)
			result[i] = aminoSequence[i].AsCharacter();

		return result;
	}

	const std::vector<std::string> BakeProteinCandidates(const EFrame frame) const
	{
		const auto& proteinCandidates{ GetFrame(frame).ProteinCandidates };

		std::vector<std::string> results;
		results.reserve(proteinCandidates.size());
		for (const auto& proteinCandidate : proteinCandidates)
		{
			const size_t proteinSize{ proteinCandidate.size() };
			std::string result{};
			result.resize(proteinSize);

			for (size_t i{ 0U }; i < proteinSize; ++i)
				result[i] = proteinCandidate[i].AsCharacter();

			results.emplace_back(std::move(result));
		}

		return results;
	}
private:
};

struct AminoMetadata final : public SequenceMetadata<AminoMetadata>
{
	std::string SequenceName;
	Bio::AminoSequence AminoSequence;
	std::vector<Bio::AminoSequence> ProteinCandidates;

	const std::string& GetName() const
	{
		return SequenceName;
	}

	AminoMetadata() = default;
	AminoMetadata(const std::string& sequenceName)
		:
		SequenceName(sequenceName)
	{}

	static inline AminoMetadata Create(const std::string& sequenceName, const std::string_view sequence)
	{
		AminoMetadata metaData(sequenceName);
		metaData.AminoSequence = Bio::ConvertToAminoSequence(sequence);

		auto& aminoSequence{ metaData.AminoSequence };
		auto& proteinCandidates{ metaData.ProteinCandidates };

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

		return metaData;
	}

	const std::string BakeSequence([[maybe_unused]] const EFrame frame = EFrame::Frame1) const
	{
		const auto& aminoSequence{ AminoSequence };
		const size_t aminoSequenceSize{ aminoSequence.size() };

		std::string result;
		result.resize(aminoSequenceSize);
		for (size_t i{ 0U }; i < aminoSequenceSize; ++i)
			result[i] = aminoSequence[i].AsCharacter();

		return result;
	}

	const std::string BakeAminoSequence([[maybe_unused]] const EFrame frame = EFrame::Frame1) const
	{
		return BakeSequence(frame);
	}

	const std::vector<std::string> BakeProteinCandidates([[maybe_unused]] const EFrame frame = EFrame::Frame1) const
	{
		const auto& proteinCandidates{ ProteinCandidates };

		std::vector<std::string> results;
		results.reserve(proteinCandidates.size());
		for (const auto& proteinCandidate : proteinCandidates)
		{
			const size_t proteinSize{ proteinCandidate.size() };
			std::string result{};
			result.resize(proteinSize);

			for (size_t i{ 0U }; i < proteinSize; ++i)
				result[i] = proteinCandidate[i].AsCharacter();

			results.emplace_back(std::move(result));
		}

		return results;
	}
};

class Project
{
private:
	NON_COPYABLE(Project)
	using SequenceTypes = std::variant
	<
	DnaMetadata,
	RnaMetadata,
	AminoMetadata
	>;
public:
	explicit Project() noexcept = default;
	~Project() noexcept;

	static std::unique_ptr<Project>& Create();
	static std::unique_ptr<Project>& Get();
private:
	static ID GenerateUUID() noexcept;

	static inline std::unordered_map<ID, SequenceTypes> s_SequenceRegistry;
	static inline std::vector<std::function<void()>>	m_SelectionContextUpdateCallbacks;
	static inline bool									m_WasUpdated{ false };
public:
	template<typename SequenceMetadataType>
	static inline SequenceTypes& RegisterSequence(const std::string& name)
	{
		const ID uuid{ GenerateUUID() };
		s_SequenceRegistry[uuid] = SequenceMetadataType{ name };

		m_WasUpdated = true;
		return s_SequenceRegistry[uuid];
	}

	static inline void UnregisterSequence(const ID uuid)
	{
		m_WasUpdated = true;
		BIO_ASSERT(uuid != g_InvalidID);
		s_SequenceRegistry.erase(uuid);
	}

	static inline void SubscribeContextSelection(const std::function<void()> function)
	{
		m_SelectionContextUpdateCallbacks.emplace_back(function);
	}

	static inline bool IsUnsaved() noexcept
	{
		return m_WasUpdated;
	}

	static inline void SetSaveStatus(const bool saved) noexcept
	{
		m_WasUpdated = !saved;
	}

	static inline void ResetCache()
	{
		s_NucleotideSequenceCache.reset();
		s_NucleotideSequencePeptideCache.reset();
		s_AminoSequenceCache.reset();
		s_AminoSequencePeptideCache.reset();
	}

	static inline void Reset()
	{
		m_WasUpdated = false;
		ResetCache();	
		s_SelectionContext.Clear();
		s_SequenceRegistry.clear();
	}

	static inline SequenceTypes& GetSequence(const ID uuid)
	{
		BIO_ASSERT(uuid != g_InvalidID && s_SequenceRegistry.contains(uuid));
		return s_SequenceRegistry[uuid];
	}

	struct SequenceIterator
	{
		std::unordered_map<ID, SequenceTypes>::const_iterator BeginIterator;
		std::unordered_map<ID, SequenceTypes>::const_iterator EndIterator;

		inline std::unordered_map<ID, SequenceTypes>::const_iterator begin() noexcept
		{
			return BeginIterator;
		}

		inline std::unordered_map<ID, SequenceTypes>::const_iterator end() noexcept
		{
			return EndIterator;
		}
	};

	static inline SequenceIterator IterateSequences()
	{
		return { s_SequenceRegistry.cbegin(), s_SequenceRegistry.cend() };
	}

	struct IsoelectricPointPlotData_t
	{
		std::vector<double> TestValues;		/* Tested pH values */
		std::vector<double> TestResults;	/* Resulting pl values */
	};

	struct HydropathyPlotData_t
	{
		std::vector<double> HydropathyIndices;
		double MaxScore, MinScore;
	};

	struct AtomicCompositionPlotData_t
	{
		const Bio::PeptideFormula& Formula;
	};

	struct CalculationSettingsContext
	{
		struct  
		{
			float PH{ 7.0f };
		} NetCharge;

		struct
		{
			size_t WindowSize{ 3U };
		} Hydropathy;
	};

	struct NucleotideSequenceCache
	{
		std::string SequenceName;
		std::string NucleotideSequence;
		std::string AminoSequence;
		std::size_t FrameIndex;

		std::vector<std::string> ProteinCandidates;
		std::vector<std::uint32_t> ProteinCandidateLengths;

		/* Properties */
		std::optional<double> MolecularWeight;
		std::optional<double> IsoeletricPoint;
		std::optional<double> NetCharge;

		/* Structure */
		std::optional<Bio::PeptideFormula> Formula;
		/* Plot data */
		std::optional<std::shared_ptr<IsoelectricPointPlotData_t>> IsoeletricPointPlotData;
		std::optional<std::shared_ptr<HydropathyPlotData_t>> HydropathyPlotData;
	};

	struct NucleotideSequencePeptideCache
	{
		NucleotideSequenceCache& ParentCache;

		std::string SequenceName;
		std::string ProteinCandidate;
		std::string AminoSequenceThreeLetterCode;
		std::size_t PeptideIndex;

		/* Properties */
		std::optional<double> MolecularWeight;
		std::optional<double> IsoeletricPoint;
		std::optional<double> NetCharge;
		size_t ExtinctionCoefficient;
		size_t ExtinctionCoefficientReduced;
		/* Structure */
		std::optional<Bio::PeptideFormula> Formula;
		/* Plot data */
		std::optional<std::shared_ptr<IsoelectricPointPlotData_t>> IsoeletricPointPlotData;
		std::optional<std::shared_ptr<HydropathyPlotData_t>> HydropathyPlotData;
	};

	struct AminoSequenceCache
	{
		std::string SequenceName;
		std::string AminoSequence;
		std::string AminoSequenceThreeLetterCode;

		std::vector<std::string> ProteinCandidates;
		std::vector<std::uint32_t> ProteinCandidateLengths;

		/* Properties */
		std::optional<double> MolecularWeight;
		std::optional<double> IsoeletricPoint;
		std::optional<double> NetCharge;
		std::size_t ExtinctionCoefficient;
		std::size_t ExtinctionCoefficientReduced;
		/* Structure */
		std::optional<Bio::PeptideFormula> Formula;
		/* Plot data */
		std::optional<std::shared_ptr<IsoelectricPointPlotData_t>> IsoeletricPointPlotData;
		std::optional<std::shared_ptr<HydropathyPlotData_t>> HydropathyPlotData;
	};
	
	struct AminoSequencePeptideCache
	{
		AminoSequenceCache& ParentCache;

		std::string SequenceName;
		std::string ProteinCandidate;
		std::string AminoSequenceThreeLetterCode;

		std::size_t PeptideIndex;

		/* Properties */
		std::optional<double> MolecularWeight;
		std::optional<double> IsoeletricPoint;
		std::optional<double> NetCharge;
		std::size_t ExtinctionCoefficient;
		std::size_t ExtinctionCoefficientReduced;
		/* Structure */
		std::optional<Bio::PeptideFormula> Formula;
		/* Plot data */
		std::optional<std::shared_ptr<IsoelectricPointPlotData_t>> IsoeletricPointPlotData;
		std::optional<std::shared_ptr<HydropathyPlotData_t>> HydropathyPlotData;
	};
private:
	constinit static inline CalculationSettingsContext s_CalculationContext;

	static inline std::optional<NucleotideSequenceCache> s_NucleotideSequenceCache;
	static inline std::optional<NucleotideSequencePeptideCache> s_NucleotideSequencePeptideCache;

	static inline std::optional<AminoSequenceCache> s_AminoSequenceCache;
	static inline std::optional<AminoSequencePeptideCache> s_AminoSequencePeptideCache;
public:
	[[nodiscard]] static CalculationSettingsContext& GetCalculationContext() noexcept;
	static void RecalculateHydropathy() noexcept;
	static void RecalculateNetCharge() noexcept;

	[[maybe_unused]] static bool OnSequenceSelected(
		const std::function<void(const NucleotideSequenceCache&)> onNucleotideSequenceSelected					= nullptr,
		const std::function<void(const NucleotideSequencePeptideCache&)> onNucleotideSequencePeptideSelected	= nullptr,
		const std::function<void(const AminoSequenceCache&)> onAminoSequenceSelected							= nullptr,
		const std::function<void(const AminoSequencePeptideCache&)> onAminoSequencePeptideSelected				= nullptr);

	[[nodiscard]] static std::filesystem::path GetWorkingPath();
private:
	enum class ESequenceSelectionType
	{
		None,
		NucleotideSequence,
		AminoSequence,
	};

	struct SelectionContext
	{
		ESequenceSelectionType Type;
		IDWrapper Sequence{ g_InvalidID };
		IDWrapper Frame{ g_InvalidID };
		IDWrapper Peptide{ g_InvalidID };

		void Clear()
		{
			Sequence = g_InvalidID;
			Frame	 = g_InvalidID;
			Peptide	 = g_InvalidID;
		}
	} inline static s_SelectionContext{};
public:
	static void InvalidateSelectionContext(
		const ESequenceSelectionType selectionType, 
		const ID sequenceID = g_InvalidID, 
		const ID frameIndex = g_InvalidID, 
		const ID peptideID = g_InvalidID);
private:
	static inline ESequenceSelectionType SelectedSequenceType() noexcept
	{
		return s_SelectionContext.Type;
	}

	static inline IDWrapper SelectedSequence() noexcept
	{
		return s_SelectionContext.Sequence;
	}

	static inline IDWrapper SelectedFrame() noexcept
	{
		return s_SelectionContext.Frame;
	}

	static inline IDWrapper SelectedPeptide() noexcept
	{
		return s_SelectionContext.Peptide;
	}
	
	static inline std::function<std::filesystem::path()> s_PathCallbackFunction;
	static void SetPathCallback(const std::function<std::filesystem::path()> callback) noexcept;
private:
	friend class ProjectSerializer;
	friend class Wizualizator;
	friend class ContentPanel;
	friend class PlotPanel;
	friend class ProjectPanel;
	friend class StructurePanel;
};

class ProjectSerializer
{
private:
	NON_COPYABLE(ProjectSerializer)
public:
	explicit ProjectSerializer(std::unique_ptr<Project>& project) noexcept;
	~ProjectSerializer() noexcept = default;

	void OnSerialize(const std::filesystem::path& path) const;
	void OnDeserialize(const std::filesystem::path& path);
private:
	std::unique_ptr<Project>& m_Project;

	static constexpr const char s_SequenceTypeToken{ '#' };
	static constexpr const char* s_DNASequenceTypeToken{ "DNA:" };
	static constexpr const char* s_RNASequenceTypeToken{ "RNA:" };
	static constexpr const char* s_PeptideSequenceTypeToken{ "PEP:" };
	static constexpr const char s_BeginLoopToken{ '{' };
	static constexpr const char s_EndLoopToken{ '}' };
};