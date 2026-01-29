#pragma once

#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <optional>
#include <expected>

namespace Unvirt::CmdHelper {

#pragma region Delegate Parameters

	enum class LoadStage { Shallow, Deep };

	struct LoadParam {
		LoadStage stage;
		std::u8string filepath;
	};

	struct UnloadParam {};

	struct SaveParam {
		std::u8string filepath;
	};

	struct InfoParam {};

	enum class LsPart {
		Object,
		Manager,
		Search,
	};

	struct LsParam {
		LsPart part;
		size_t page; ///< One-based page.
	};

	enum class DataPart {
		Object,
		Manager,
	};

	struct DataParam {
		DataPart part;
		size_t index; ///< Zero-based index
	};

	enum class ChunkPart {
		Object,
		Manager,
	};

	struct ChunkParam {
		ChunkPart part;
		size_t index; ///< Zero-based index
	};

	enum class SearchPart {
		Object,
		Manager,
	};

	enum class SearchMode {
		PlainText,
		Regex,
	};

	struct SearchParam {
		SearchPart part;
		SearchMode mode;
		std::u8string text;
	};

	struct ItemsParam {
		size_t count; ///< The lowest value is one.
	};

	enum class StyleLevel {
		Full,
		Simple,
	};

	struct StyleParam {
		StyleLevel level;
	};

	struct EncodingParam {
		std::vector<std::u8string> enc;
	};

	struct TempParam {
		std::u8string dirpath;
	};

	struct RscClearParam {};
	struct RscAddParam {
		std::u8string dirpath;
	};

	struct TestParam {};

	struct VersionParam {};

	struct HelpParam {};

	struct ExitParam {};

#pragma endregion

#pragma region Delegates

	using LoadDelegate = std::function<void(const LoadParam &)>;
	using UnloadDelegate = std::function<void(const UnloadParam &)>;
	using SaveDelegate = std::function<void(const SaveParam &)>;
	using InfoDelegate = std::function<void(const InfoParam &)>;
	using LsDelegate = std::function<void(const LsParam &)>;
	using DataDelegate = std::function<void(const DataParam &)>;
	using ChunkDelegate = std::function<void(const ChunkParam &)>;
	using SearchDelegate = std::function<void(const SearchParam &)>;
	using ItemsDelegate = std::function<void(const ItemsParam &)>;
	using StyleDelegate = std::function<void(const StyleParam &)>;
	using EncodingDelegate = std::function<void(const EncodingParam &)>;
	using TempDelegate = std::function<void(const TempParam &)>;
	using RscClearDelegate = std::function<void(const RscClearParam &)>;
	using RscAddDelegate = std::function<void(const RscAddParam &)>;
	using TestDelegate = std::function<void(const TestParam &)>;
	using VersionDelegate = std::function<void(const VersionParam &)>;
	using HelpDelegate = std::function<void(const HelpParam &)>;
	using ExitDelegate = std::function<void(const ExitParam &)>;

#pragma endregion

#pragma region Error Types

	enum class Error {
		Lexer,        ///< Fail to do lexer.
		TooMuchParam, ///< The count of parameter is larger than expected.
		TooLessParam, ///< The count of parameter is to less as expected.
		BadArg,       ///< The format of argument is wrong.
		BadVerb,      ///< Given verb do not match with any existing items.
	};

	template<typename T>
	using Result = std::expected<T, Error>;

#pragma endregion

	class ParamStack {
	public:
		ParamStack(std::vector<std::u8string>&& params);
		~ParamStack();
		YYCC_DEFAULT_COPY_MOVE(ParamStack)

	public:
		std::optional<std::u8string_view> Next();

	private:
		std::vector<std::u8string> params;
		size_t cursor;
	};

	class Commander {
	public:
		Commander();
		~Commander();
		YYCC_DELETE_COPY_MOVE(Commander)

	public:
		Result<void> Dispatch(const std::u8string_view &cmd) const;

	private:
		Result<ParamStack> BuildStack(const std::u8string_view &cmd) const;

	private:
		Result<void> Branch(ParamStack &stack) const;
		Result<void> LoadBranch(ParamStack &stack) const;
		Result<void> UnloadBranch(ParamStack &stack) const;
		Result<void> SaveBranch(ParamStack &stack) const;
		Result<void> InfoBranch(ParamStack &stack) const;
		Result<void> LsBranch(ParamStack &stack) const;
		Result<void> DataBranch(ParamStack &stack) const;
		Result<void> ChunkBranch(ParamStack &stack) const;
		Result<void> SearchBranch(ParamStack &stack) const;
		Result<void> ItemsBranch(ParamStack &stack) const;
		Result<void> StyleBranch(ParamStack &stack) const;
		Result<void> EncodingBranch(ParamStack &stack) const;
		Result<void> TempBranch(ParamStack &stack) const;
		Result<void> RscBranch(ParamStack &stack) const;
		Result<void> RscClearBranch(ParamStack &stack) const;
		Result<void> RscAddBranch(ParamStack &stack) const;
		Result<void> TestBranch(ParamStack &stack) const;
		Result<void> VersionBranch(ParamStack &stack) const;
		Result<void> HelpBranch(ParamStack &stack) const;
		Result<void> ExitBranch(ParamStack &stack) const;

	public:
		void SetLoadDelegate(LoadDelegate &&delegate);
		void SetUnloadDelegate(UnloadDelegate &&delegate);
		void SetSaveDelegate(SaveDelegate &&delegate);
		void SetInfoDelegate(InfoDelegate &&delegate);
		void SetLsDelegate(LsDelegate &&delegate);
		void SetDataDelegate(DataDelegate &&delegate);
		void SetChunkDelegate(ChunkDelegate &&delegate);
		void SetSearchDelegate(SearchDelegate &&delegate);
		void SetItemsDelegate(ItemsDelegate &&delegate);
		void SetStyleDelegate(StyleDelegate &&delegate);
		void SetEncodingDelegate(EncodingDelegate &&delegate);
		void SetTempDelegate(TempDelegate &&delegate);
		void SetRscClearDelegate(RscClearDelegate &&delegate);
		void SetRscAddDelegate(RscAddDelegate &&delegate);
		void SetTestDelegate(TestDelegate &&delegate);
		void SetVersionDelegate(VersionDelegate &&delegate);
		void SetHelpDelegate(HelpDelegate &&delegate);
		void SetExitDelegate(ExitDelegate &&delegate);

	private:
		LoadDelegate load_delegate;
		UnloadDelegate unload_delegate;
		SaveDelegate save_delegate;
		InfoDelegate info_delegate;
		LsDelegate ls_delegate;
		DataDelegate data_delegate;
		ChunkDelegate chunk_delegate;
		SearchDelegate search_delegate;
		ItemsDelegate items_delegate;
		StyleDelegate style_delegate;
		EncodingDelegate encoding_delegate;
		TempDelegate temp_delegate;
		RscClearDelegate rsc_clear_delegate;
		RscAddDelegate rsc_add_delegate;
		TestDelegate test_delegate;
		VersionDelegate version_delegate;
		HelpDelegate help_delegate;
		ExitDelegate exit_delegate;
	};

} // namespace Unvirt::CmdHelper
