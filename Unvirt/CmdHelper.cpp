#include "CmdHelper.hpp"
#include <yycc/carton/lexer61.hpp>
#include <yycc/num/parse.hpp>

namespace Unvirt::CmdHelper {

#pragma region Enums Parser

	static std::optional<LoadStage> ParseLoadStage(const std::u8string_view &sv) {
		if (sv == u8"shallow") return LoadStage::Shallow;
		else if (sv == u8"deel") return LoadStage::Deep;
		else return std::nullopt;
	}

	static std::optional<LsPart> ParseLsPart(const std::u8string_view &sv) { 
		if (sv == u8"mgr") return LsPart::Manager;
		else if (sv == u8"obj") return LsPart::Object;
		else if (sv == u8"search") return LsPart::Search;
		else return std::nullopt;
	}

	static std::optional<DataPart> ParseDataPart(const std::u8string_view &sv) { 
		if (sv == u8"mgr") return DataPart::Manager;
		else if (sv == u8"obj") return DataPart::Object;
		else return std::nullopt;
	}

	static std::optional<ChunkPart> ParseChunkPart(const std::u8string_view &sv) { 
		if (sv == u8"mgr") return ChunkPart::Manager;
		else if (sv == u8"obj") return ChunkPart::Object;
		else return std::nullopt;
	}

	static std::optional<SearchPart> ParseSearchPart(const std::u8string_view &sv) { 
		if (sv == u8"mgr") return SearchPart::Manager;
		else if (sv == u8"obj") return SearchPart::Object;
		else return std::nullopt;
	}

	static std::optional<SearchMode> ParseSearchMode(const std::u8string_view &sv) {
		if (sv == u8"plain") return SearchMode::PlainText;
		else if (sv == u8"re") return SearchMode::Regex;
		else return std::nullopt;
	}

	static std::optional<StyleLevel> ParseStyleLevel(const std::u8string_view &sv) { 
		if (sv == u8"simple") return StyleLevel::Simple;
		else if (sv == u8"full") return StyleLevel::Full;
		else return std::nullopt;
	}

#pragma endregion

#pragma region Parameter Stack

	ParamStack::ParamStack(std::vector<std::u8string> &&params) : params(std::move(params)), cursor(0) {}

	ParamStack::~ParamStack() {}

	std::optional<std::u8string_view> ParamStack::Next() {
		if (this->cursor >= this->params.size()) {
			return std::nullopt;
		} else {
			return this->params[this->cursor++];
		}
	}

#pragma endregion

#pragma region Commander

	Commander::Commander() {}

	Commander::~Commander() {}

	Result<void> Commander::Dispatch(const std::u8string_view &cmd) const {
		auto stack = this->BuildStack(cmd);
		if (stack.has_value()) {
			return this->Branch(stack.value());
		} else {
			return std::unexpected(stack.error());
		}
	}

	Result<ParamStack> Commander::BuildStack(const std::u8string_view &cmd) const {
		yycc::carton::lexer61::Lexer61 lexer;
		auto rv = lexer.lex(cmd);
		if (rv.has_value()) {
			return ParamStack(std::move(rv.value()));
		} else {
			return std::unexpected(Error::Lexer);
		}
	}

#pragma region Branch

#define CHECK_NO_MORE_PARAM(stack) \
	if (stack.Next().has_value()) { \
		return std::unexpected(Error::TooMuchParam); \
	}
#define SAFE_CALL_DELEGATE(delegate, payload) \
	if ((delegate) != nullptr) { \
		(delegate)(payload); \
	}

	Result<void> Commander::Branch(ParamStack &stack) const {
		auto param = stack.Next();
		if (param.has_value()) {
			const auto &verb = param.value();

			if (verb == u8"load") {
				return this->LoadBranch(stack);
			} else if (verb == u8"unload") {
				return this->UnloadBranch(stack);
			} else if (verb == u8"save") {
				return this->SaveBranch(stack);
			} else if (verb == u8"info") {
				return this->InfoBranch(stack);
			} else if (verb == u8"ls") {
				return this->LsBranch(stack);
			} else if (verb == u8"data") {
				return this->DataBranch(stack);
			} else if (verb == u8"chunk") {
				return this->ChunkBranch(stack);
			} else if (verb == u8"search") {
				return this->SearchBranch(stack);
			} else if (verb == u8"items") {
				return this->ItemsBranch(stack);
			} else if (verb == u8"style") {
				return this->StyleBranch(stack);
			} else if (verb == u8"encoding") {
				return this->EncodingBranch(stack);
			} else if (verb == u8"temp") {
				return this->TempBranch(stack);
			} else if (verb == u8"rsc") {
				return this->RscBranch(stack);
			} else if (verb == u8"test") {
				return this->TestBranch(stack);
			} else if (verb == u8"version") {
				return this->VersionBranch(stack);
			} else if (verb == u8"help") {
				return this->HelpBranch(stack);
			} else if (verb == u8"exit") {
				return this->ExitBranch(stack);
			} else {
				return std::unexpected(Error::BadVerb);
			}
		} else {
			return std::unexpected(Error::TooLessParam);
		}
	}
	Result<void> Commander::LoadBranch(ParamStack &stack) const {
		auto stage_param = stack.Next();
		if (!stage_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto stage_arg = ParseLoadStage(stage_param.value());
		if (!stage_arg.has_value()) return std::unexpected(Error::BadArg);

		auto filepath_arg = stack.Next();
		if (!filepath_arg.has_value()) return std::unexpected(Error::TooLessParam);

		CHECK_NO_MORE_PARAM(stack)

		LoadParam param {
			.stage = stage_arg.value(),
		    .filepath = std::u8string(filepath_arg.value()),
		};
		SAFE_CALL_DELEGATE(this->load_delegate, param)
		return {};
	}
	Result<void> Commander::UnloadBranch(ParamStack &stack) const {
		CHECK_NO_MORE_PARAM(stack)
		SAFE_CALL_DELEGATE(this->unload_delegate, UnloadParam{})
		return {};
	}
	Result<void> Commander::SaveBranch(ParamStack &stack) const {
		auto filepath_arg = stack.Next();
		if (!filepath_arg.has_value()) return std::unexpected(Error::TooLessParam);

		CHECK_NO_MORE_PARAM(stack)

		SaveParam param {
			.filepath = std::u8string(filepath_arg.value()),
		};
		SAFE_CALL_DELEGATE(this->save_delegate, param)
		return {};
	}
	Result<void> Commander::InfoBranch(ParamStack &stack) const {
		CHECK_NO_MORE_PARAM(stack)
		SAFE_CALL_DELEGATE(this->info_delegate, InfoParam{})
		return {};
	}
	Result<void> Commander::LsBranch(ParamStack &stack) const {
		auto part_param = stack.Next();
		if (!part_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto part_arg = ParseLsPart(part_param.value());
		if (!part_arg.has_value()) return std::unexpected(Error::BadArg);

		auto page_param = stack.Next();
		if (!page_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto page_arg = yycc::num::parse::parse<size_t>(page_param.value());
		if (!page_arg.has_value()) return std::unexpected(Error::BadArg);
		if (page_arg.value() < 1) return std::unexpected(Error::BadArg);

		CHECK_NO_MORE_PARAM(stack)

		LsParam param {
			.part = part_arg.value(),
			.page = page_arg.value(),
		};
		SAFE_CALL_DELEGATE(this->ls_delegate, param)
		return {};
	}
	Result<void> Commander::DataBranch(ParamStack &stack) const {
		auto part_param = stack.Next();
		if (!part_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto part_arg = ParseDataPart(part_param.value());
		if (!part_arg.has_value()) return std::unexpected(Error::BadArg);

		auto index_param = stack.Next();
		if (!index_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto index_arg = yycc::num::parse::parse<size_t>(index_param.value());
		if (!index_arg.has_value()) return std::unexpected(Error::BadArg);

		CHECK_NO_MORE_PARAM(stack)

		DataParam param {
			.part = part_arg.value(),
			.index = index_arg.value(),
		};
		SAFE_CALL_DELEGATE(this->data_delegate, param)
		return {};
	}
	Result<void> Commander::ChunkBranch(ParamStack &stack) const {
		auto part_param = stack.Next();
		if (!part_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto part_arg = ParseChunkPart(part_param.value());
		if (!part_arg.has_value()) return std::unexpected(Error::BadArg);

		auto index_param = stack.Next();
		if (!index_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto index_arg = yycc::num::parse::parse<size_t>(index_param.value());
		if (!index_arg.has_value()) return std::unexpected(Error::BadArg);

		CHECK_NO_MORE_PARAM(stack)

		ChunkParam param {
			.part = part_arg.value(),
			.index = index_arg.value(),
		};
		SAFE_CALL_DELEGATE(this->chunk_delegate, param)
		return {};
	}
	Result<void> Commander::SearchBranch(ParamStack &stack) const {
		auto part_param = stack.Next();
		if (!part_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto part_arg = ParseSearchPart(part_param.value());
		if (!part_arg.has_value()) return std::unexpected(Error::BadArg);

		auto mode_param = stack.Next();
		if (!mode_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto mode_arg = ParseSearchMode(mode_param.value());
		if (!mode_arg.has_value()) return std::unexpected(Error::BadArg);

		auto text_arg = stack.Next();
		if (!text_arg.has_value()) return std::unexpected(Error::TooLessParam);

		CHECK_NO_MORE_PARAM(stack)

		SearchParam param {
			.part = part_arg.value(),
			.mode = mode_arg.value(),
			.text = std::u8string(text_arg.value()),
		};
		SAFE_CALL_DELEGATE(this->search_delegate, param)
		return {};
	}
	Result<void> Commander::ItemsBranch(ParamStack &stack) const {
		auto count_param = stack.Next();
		if (!count_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto count_arg = yycc::num::parse::parse<size_t>(count_param.value());
		if (!count_arg.has_value()) return std::unexpected(Error::BadArg);
		if (count_arg.value() < 1) return std::unexpected(Error::BadArg);

		CHECK_NO_MORE_PARAM(stack)

		ItemsParam param {
			.count = count_arg.value(),
		};
		SAFE_CALL_DELEGATE(this->items_delegate, param)
		return {};
	}
	Result<void> Commander::StyleBranch(ParamStack &stack) const {
		auto level_param = stack.Next();
		if (!level_param.has_value()) return std::unexpected(Error::TooLessParam);
		auto level_arg = ParseStyleLevel(level_param.value());
		if (!level_arg.has_value()) return std::unexpected(Error::BadArg);

		CHECK_NO_MORE_PARAM(stack)

		StyleParam param {
			.level = level_arg.value(),
		};
		SAFE_CALL_DELEGATE(this->style_delegate, param)
		return {};
	}
	Result<void> Commander::EncodingBranch(ParamStack &stack) const {
		std::vector<std::u8string> collector;
		while (true) {
			auto param = stack.Next();
			if (param.has_value()) collector.emplace_back(param.value());
			else break;
		}
		if (collector.empty()) return std::unexpected(Error::TooLessParam);

		EncodingParam param{
		    .enc = std::move(collector),
		};
		SAFE_CALL_DELEGATE(this->encoding_delegate, param)
		return {};
	}
	Result<void> Commander::TempBranch(ParamStack &stack) const {
		auto dirpath_arg = stack.Next();
		if (!dirpath_arg.has_value()) return std::unexpected(Error::TooLessParam);

		CHECK_NO_MORE_PARAM(stack)

		TempParam param {
			.dirpath = std::u8string(dirpath_arg.value()),
		};
		SAFE_CALL_DELEGATE(this->temp_delegate, param)
		return {};
	}
	Result<void> Commander::RscBranch(ParamStack &stack) const {
		auto param = stack.Next();
		if (param.has_value()) {
			const auto &verb = param.value();

			if (verb == u8"clear") {
				return this->RscClearBranch(stack);
			} else if (verb == u8"add") {
				return this->RscAddBranch(stack);
			} else {
				return std::unexpected(Error::BadVerb);
			}
		} else {
			return std::unexpected(Error::TooLessParam);
		}
	}
	Result<void> Commander::RscClearBranch(ParamStack &stack) const {
		CHECK_NO_MORE_PARAM(stack)
		SAFE_CALL_DELEGATE(this->rsc_clear_delegate, RscClearParam{})
		return {};
	}
	Result<void> Commander::RscAddBranch(ParamStack &stack) const {
		auto dirpath_arg = stack.Next();
		if (!dirpath_arg.has_value()) return std::unexpected(Error::TooLessParam);

		CHECK_NO_MORE_PARAM(stack)

		RscAddParam param {
			.dirpath = std::u8string(dirpath_arg.value()),
		};
		SAFE_CALL_DELEGATE(this->rsc_add_delegate, param)
		return {};
	}
	Result<void> Commander::TestBranch(ParamStack &stack) const {
		CHECK_NO_MORE_PARAM(stack)
		SAFE_CALL_DELEGATE(this->test_delegate, TestParam{})
		return {};
	}
	Result<void> Commander::VersionBranch(ParamStack &stack) const {
		CHECK_NO_MORE_PARAM(stack)
		SAFE_CALL_DELEGATE(this->version_delegate, VersionParam{})
		return {};
	}
	Result<void> Commander::HelpBranch(ParamStack &stack) const {
		CHECK_NO_MORE_PARAM(stack)
		SAFE_CALL_DELEGATE(this->help_delegate, HelpParam{})
		return {};
	}
	Result<void> Commander::ExitBranch(ParamStack &stack) const {
		CHECK_NO_MORE_PARAM(stack)
		SAFE_CALL_DELEGATE(this->exit_delegate, ExitParam{})
		return {};
	}

#undef SAFE_CALL_DELEGATE
#undef CHECK_NO_MORE_PARAM

#pragma endregion

#pragma region Delegate Setter

	void Commander::SetLoadDelegate(LoadDelegate &&delegate) {
		this->load_delegate = std::move(delegate);
	}
	void Commander::SetUnloadDelegate(UnloadDelegate &&delegate) {
		this->unload_delegate = std::move(delegate);
	}
	void Commander::SetSaveDelegate(SaveDelegate &&delegate) {
		this->save_delegate = std::move(delegate);
	}
	void Commander::SetInfoDelegate(InfoDelegate &&delegate) {
		this->info_delegate = std::move(delegate);
	}
	void Commander::SetLsDelegate(LsDelegate &&delegate) {
		this->ls_delegate = std::move(delegate);
	}
	void Commander::SetDataDelegate(DataDelegate &&delegate) {
		this->data_delegate = std::move(delegate);
	}
	void Commander::SetChunkDelegate(ChunkDelegate &&delegate) {
		this->chunk_delegate = std::move(delegate);
	}
	void Commander::SetSearchDelegate(SearchDelegate &&delegate) {
		this->search_delegate = std::move(delegate);
	}
	void Commander::SetItemsDelegate(ItemsDelegate &&delegate) {
		this->items_delegate = std::move(delegate);
	}
	void Commander::SetStyleDelegate(StyleDelegate &&delegate) {
		this->style_delegate = std::move(delegate);
	}
	void Commander::SetEncodingDelegate(EncodingDelegate &&delegate) {
		this->encoding_delegate = std::move(delegate);
	}
	void Commander::SetTempDelegate(TempDelegate &&delegate) {
		this->temp_delegate = std::move(delegate);
	}
	void Commander::SetRscClearDelegate(RscClearDelegate &&delegate) {
		this->rsc_clear_delegate = std::move(delegate);
	}
	void Commander::SetRscAddDelegate(RscAddDelegate &&delegate) {
		this->rsc_add_delegate = std::move(delegate);
	}
	void Commander::SetTestDelegate(TestDelegate &&delegate) {
		this->test_delegate = std::move(delegate);
	}
	void Commander::SetVersionDelegate(VersionDelegate &&delegate) {
		this->version_delegate = std::move(delegate);
	}
	void Commander::SetHelpDelegate(HelpDelegate &&delegate) {
		this->help_delegate = std::move(delegate);
	}
	void Commander::SetExitDelegate(ExitDelegate &&delegate) {
		this->exit_delegate = std::move(delegate);
	}

#pragma endregion

#pragma endregion

} // namespace Unvirt::CmdHelper
