#include "VTEncoding.hpp"
#include <map>

#if YYCC_OS == YYCC_OS_WINDOWS
#include <WinImportPrefix.hpp>
#include <Windows.h>
#include <fileapi.h>
#include <WinImportSuffix.hpp>
#else
#include <iconv.h>
#endif

namespace LibCmo::EncodingHelper {

#pragma region Constant Map

	static const std::map<std::u8string, std::u8string> c_AliasMap {
		{ u8"646", u8"ascii" },
		{ u8"us-ascii", u8"ascii" },
		{ u8"big5-tw", u8"big5" },
		{ u8"csbig5", u8"big5" },
		{ u8"big5-hkscs", u8"big5hkscs" },
		{ u8"hkscs", u8"big5hkscs" },
		{ u8"ibm037", u8"cp037" },
		{ u8"ibm039", u8"cp037" },
		{ u8"273", u8"cp273" },
		{ u8"ibm273", u8"cp273" },
		{ u8"csibm273", u8"cp273" },
		{ u8"ebcdic-cp-he", u8"cp424" },
		{ u8"ibm424", u8"cp424" },
		{ u8"437", u8"cp437" },
		{ u8"ibm437", u8"cp437" },
		{ u8"ebcdic-cp-be", u8"cp500" },
		{ u8"ebcdic-cp-ch", u8"cp500" },
		{ u8"ibm500", u8"cp500" },
		{ u8"ibm775", u8"cp775" },
		{ u8"850", u8"cp850" },
		{ u8"ibm850", u8"cp850" },
		{ u8"852", u8"cp852" },
		{ u8"ibm852", u8"cp852" },
		{ u8"855", u8"cp855" },
		{ u8"ibm855", u8"cp855" },
		{ u8"857", u8"cp857" },
		{ u8"ibm857", u8"cp857" },
		{ u8"858", u8"cp858" },
		{ u8"ibm858", u8"cp858" },
		{ u8"860", u8"cp860" },
		{ u8"ibm860", u8"cp860" },
		{ u8"861", u8"cp861" },
		{ u8"cp-is", u8"cp861" },
		{ u8"ibm861", u8"cp861" },
		{ u8"862", u8"cp862" },
		{ u8"ibm862", u8"cp862" },
		{ u8"863", u8"cp863" },
		{ u8"ibm863", u8"cp863" },
		{ u8"ibm864", u8"cp864" },
		{ u8"865", u8"cp865" },
		{ u8"ibm865", u8"cp865" },
		{ u8"866", u8"cp866" },
		{ u8"ibm866", u8"cp866" },
		{ u8"869", u8"cp869" },
		{ u8"cp-gr", u8"cp869" },
		{ u8"ibm869", u8"cp869" },
		{ u8"932", u8"cp932" },
		{ u8"ms932", u8"cp932" },
		{ u8"mskanji", u8"cp932" },
		{ u8"ms-kanji", u8"cp932" },
		{ u8"windows-31j", u8"cp932" },
		{ u8"949", u8"cp949" },
		{ u8"ms949", u8"cp949" },
		{ u8"uhc", u8"cp949" },
		{ u8"950", u8"cp950" },
		{ u8"ms950", u8"cp950" },
		{ u8"ibm1026", u8"cp1026" },
		{ u8"1125", u8"cp1125" },
		{ u8"ibm1125", u8"cp1125" },
		{ u8"cp866u", u8"cp1125" },
		{ u8"ruscii", u8"cp1125" },
		{ u8"ibm1140", u8"cp1140" },
		{ u8"windows-1250", u8"cp1250" },
		{ u8"windows-1251", u8"cp1251" },
		{ u8"windows-1252", u8"cp1252" },
		{ u8"windows-1253", u8"cp1253" },
		{ u8"windows-1254", u8"cp1254" },
		{ u8"windows-1255", u8"cp1255" },
		{ u8"windows-1256", u8"cp1256" },
		{ u8"windows-1257", u8"cp1257" },
		{ u8"windows-1258", u8"cp1258" },
		{ u8"eucjp", u8"euc_jp" },
		{ u8"ujis", u8"euc_jp" },
		{ u8"u-jis", u8"euc_jp" },
		{ u8"jisx0213", u8"euc_jis_2004" },
		{ u8"eucjis2004", u8"euc_jis_2004" },
		{ u8"eucjisx0213", u8"euc_jisx0213" },
		{ u8"euckr", u8"euc_kr" },
		{ u8"korean", u8"euc_kr" },
		{ u8"ksc5601", u8"euc_kr" },
		{ u8"ks_c-5601", u8"euc_kr" },
		{ u8"ks_c-5601-1987", u8"euc_kr" },
		{ u8"ksx1001", u8"euc_kr" },
		{ u8"ks_x-1001", u8"euc_kr" },
		{ u8"chinese", u8"gb2312" },
		{ u8"csiso58gb231280", u8"gb2312" },
		{ u8"euc-cn", u8"gb2312" },
		{ u8"euccn", u8"gb2312" },
		{ u8"eucgb2312-cn", u8"gb2312" },
		{ u8"gb2312-1980", u8"gb2312" },
		{ u8"gb2312-80", u8"gb2312" },
		{ u8"iso-ir-58", u8"gb2312" },
		{ u8"936", u8"gbk" },
		{ u8"cp936", u8"gbk" },
		{ u8"ms936", u8"gbk" },
		{ u8"gb18030-2000", u8"gb18030" },
		{ u8"hzgb", u8"hz" },
		{ u8"hz-gb", u8"hz" },
		{ u8"hz-gb-2312", u8"hz" },
		{ u8"csiso2022jp", u8"iso2022_jp" },
		{ u8"iso2022jp", u8"iso2022_jp" },
		{ u8"iso-2022-jp", u8"iso2022_jp" },
		{ u8"iso2022jp-1", u8"iso2022_jp_1" },
		{ u8"iso-2022-jp-1", u8"iso2022_jp_1" },
		{ u8"iso2022jp-2", u8"iso2022_jp_2" },
		{ u8"iso-2022-jp-2", u8"iso2022_jp_2" },
		{ u8"iso2022jp-2004", u8"iso2022_jp_2004" },
		{ u8"iso-2022-jp-2004", u8"iso2022_jp_2004" },
		{ u8"iso2022jp-3", u8"iso2022_jp_3" },
		{ u8"iso-2022-jp-3", u8"iso2022_jp_3" },
		{ u8"iso2022jp-ext", u8"iso2022_jp_ext" },
		{ u8"iso-2022-jp-ext", u8"iso2022_jp_ext" },
		{ u8"csiso2022kr", u8"iso2022_kr" },
		{ u8"iso2022kr", u8"iso2022_kr" },
		{ u8"iso-2022-kr", u8"iso2022_kr" },
		{ u8"iso-8859-1", u8"latin_1" },
		{ u8"iso8859-1", u8"latin_1" },
		{ u8"8859", u8"latin_1" },
		{ u8"cp819", u8"latin_1" },
		{ u8"latin", u8"latin_1" },
		{ u8"latin1", u8"latin_1" },
		{ u8"l1", u8"latin_1" },
		{ u8"iso-8859-2", u8"iso8859_2" },
		{ u8"latin2", u8"iso8859_2" },
		{ u8"l2", u8"iso8859_2" },
		{ u8"iso-8859-3", u8"iso8859_3" },
		{ u8"latin3", u8"iso8859_3" },
		{ u8"l3", u8"iso8859_3" },
		{ u8"iso-8859-4", u8"iso8859_4" },
		{ u8"latin4", u8"iso8859_4" },
		{ u8"l4", u8"iso8859_4" },
		{ u8"iso-8859-5", u8"iso8859_5" },
		{ u8"cyrillic", u8"iso8859_5" },
		{ u8"iso-8859-6", u8"iso8859_6" },
		{ u8"arabic", u8"iso8859_6" },
		{ u8"iso-8859-7", u8"iso8859_7" },
		{ u8"greek", u8"iso8859_7" },
		{ u8"greek8", u8"iso8859_7" },
		{ u8"iso-8859-8", u8"iso8859_8" },
		{ u8"hebrew", u8"iso8859_8" },
		{ u8"iso-8859-9", u8"iso8859_9" },
		{ u8"latin5", u8"iso8859_9" },
		{ u8"l5", u8"iso8859_9" },
		{ u8"iso-8859-10", u8"iso8859_10" },
		{ u8"latin6", u8"iso8859_10" },
		{ u8"l6", u8"iso8859_10" },
		{ u8"iso-8859-11", u8"iso8859_11" },
		{ u8"thai", u8"iso8859_11" },
		{ u8"iso-8859-13", u8"iso8859_13" },
		{ u8"latin7", u8"iso8859_13" },
		{ u8"l7", u8"iso8859_13" },
		{ u8"iso-8859-14", u8"iso8859_14" },
		{ u8"latin8", u8"iso8859_14" },
		{ u8"l8", u8"iso8859_14" },
		{ u8"iso-8859-15", u8"iso8859_15" },
		{ u8"latin9", u8"iso8859_15" },
		{ u8"l9", u8"iso8859_15" },
		{ u8"iso-8859-16", u8"iso8859_16" },
		{ u8"latin10", u8"iso8859_16" },
		{ u8"l10", u8"iso8859_16" },
		{ u8"cp1361", u8"johab" },
		{ u8"ms1361", u8"johab" },
		{ u8"kz_1048", u8"kz1048" },
		{ u8"strk1048_2002", u8"kz1048" },
		{ u8"rk1048", u8"kz1048" },
		{ u8"maccyrillic", u8"mac_cyrillic" },
		{ u8"macgreek", u8"mac_greek" },
		{ u8"maciceland", u8"mac_iceland" },
		{ u8"maclatin2", u8"mac_latin2" },
		{ u8"maccentraleurope", u8"mac_latin2" },
		{ u8"mac_centeuro", u8"mac_latin2" },
		{ u8"macroman", u8"mac_roman" },
		{ u8"macintosh", u8"mac_roman" },
		{ u8"macturkish", u8"mac_turkish" },
		{ u8"csptcp154", u8"ptcp154" },
		{ u8"pt154", u8"ptcp154" },
		{ u8"cp154", u8"ptcp154" },
		{ u8"cyrillic-asian", u8"ptcp154" },
		{ u8"csshiftjis", u8"shift_jis" },
		{ u8"shiftjis", u8"shift_jis" },
		{ u8"sjis", u8"shift_jis" },
		{ u8"s_jis", u8"shift_jis" },
		{ u8"shiftjis2004", u8"shift_jis_2004" },
		{ u8"sjis_2004", u8"shift_jis_2004" },
		{ u8"sjis2004", u8"shift_jis_2004" },
		{ u8"shiftjisx0213", u8"shift_jisx0213" },
		{ u8"sjisx0213", u8"shift_jisx0213" },
		{ u8"s_jisx0213", u8"shift_jisx0213" },
		{ u8"u32", u8"utf_32" },
		{ u8"utf32", u8"utf_32" },
		{ u8"utf-32be", u8"utf_32_be" },
		{ u8"utf-32le", u8"utf_32_le" },
		{ u8"u16", u8"utf_16" },
		{ u8"utf16", u8"utf_16" },
		{ u8"utf-16be", u8"utf_16_be" },
		{ u8"utf-16le", u8"utf_16_le" },
		{ u8"u7", u8"utf_7" },
		{ u8"unicode-1-1-utf-7", u8"utf_7" },
		{ u8"u8", u8"utf_8" },
		{ u8"utf", u8"utf_8" },
		{ u8"utf8", u8"utf_8" },
		{ u8"cp65001", u8"utf_8" },
	};

	/**
	 * @brief Resolve encoding name alias and fetch real encoding name.
	 * @param[in] lang The encoding name for finding.
	 * @return
	 * The given encoding name if given name not present in alias map.
	 * Otherwise the found encoding name by given name.
	*/
	static std::u8string ResolveEncodingAlias(const std::u8string_view& enc_name) {
		std::u8string name(enc_name);
		YYCC::StringHelper::Lower(name);
		auto finder = c_AliasMap.find(name);
		if (finder == c_AliasMap.end()) return std::u8string(enc_name); // not found, use original encoding name.
		else return std::u8string(finder->second); // found, use found encoding name.
	}

#if YYCC_OS == YYCC_OS_WINDOWS
	
	static const std::map<std::u8string, UINT> c_WinCPMap {	
		{ u8"big5", static_cast<UINT>(950u) },
		{ u8"cp037", static_cast<UINT>(037u) },
		{ u8"cp437", static_cast<UINT>(437u) },
		{ u8"cp500", static_cast<UINT>(500u) },
		{ u8"cp720", static_cast<UINT>(720u) },
		{ u8"cp737", static_cast<UINT>(737u) },
		{ u8"cp775", static_cast<UINT>(775u) },
		{ u8"cp850", static_cast<UINT>(850u) },
		{ u8"cp852", static_cast<UINT>(852u) },
		{ u8"cp855", static_cast<UINT>(855u) },
		{ u8"cp857", static_cast<UINT>(857u) },
		{ u8"cp858", static_cast<UINT>(858u) },
		{ u8"cp860", static_cast<UINT>(860u) },
		{ u8"cp861", static_cast<UINT>(861u) },
		{ u8"cp862", static_cast<UINT>(862u) },
		{ u8"cp863", static_cast<UINT>(863u) },
		{ u8"cp864", static_cast<UINT>(864u) },
		{ u8"cp865", static_cast<UINT>(865u) },
		{ u8"cp866", static_cast<UINT>(866u) },
		{ u8"cp869", static_cast<UINT>(869u) },
		{ u8"cp874", static_cast<UINT>(874u) },
		{ u8"cp875", static_cast<UINT>(875u) },
		{ u8"cp932", static_cast<UINT>(932u) },
		{ u8"cp949", static_cast<UINT>(949u) },
		{ u8"cp950", static_cast<UINT>(950u) },
		{ u8"cp1026", static_cast<UINT>(1026u) },
		{ u8"cp1140", static_cast<UINT>(1140u) },
		{ u8"cp1250", static_cast<UINT>(1250u) },
		{ u8"cp1251", static_cast<UINT>(1251u) },
		{ u8"cp1252", static_cast<UINT>(1252u) },
		{ u8"cp1253", static_cast<UINT>(1253u) },
		{ u8"cp1254", static_cast<UINT>(1254u) },
		{ u8"cp1255", static_cast<UINT>(1255u) },
		{ u8"cp1256", static_cast<UINT>(1256u) },
		{ u8"cp1257", static_cast<UINT>(1257u) },
		{ u8"cp1258", static_cast<UINT>(1258u) },
		{ u8"euc_kr", static_cast<UINT>(51949u) },
		{ u8"gbk", static_cast<UINT>(936u) },
		{ u8"gb18030", static_cast<UINT>(54936u) },
		{ u8"hz", static_cast<UINT>(52936u) },
		{ u8"iso2022_kr", static_cast<UINT>(50225u) },
		{ u8"latin_1", static_cast<UINT>(28591u) },
		{ u8"iso8859_2", static_cast<UINT>(28592u) },
		{ u8"iso8859_3", static_cast<UINT>(28593u) },
		{ u8"iso8859_4", static_cast<UINT>(28594u) },
		{ u8"iso8859_5", static_cast<UINT>(28595u) },
		{ u8"iso8859_6", static_cast<UINT>(28596u) },
		{ u8"iso8859_7", static_cast<UINT>(28597u) },
		{ u8"iso8859_8", static_cast<UINT>(28598u) },
		{ u8"iso8859_9", static_cast<UINT>(28599u) },
		{ u8"iso8859_13", static_cast<UINT>(28603u) },
		{ u8"iso8859_15", static_cast<UINT>(28605u) },
		{ u8"johab", static_cast<UINT>(1361u) },
		{ u8"mac_cyrillic", static_cast<UINT>(10007u) },
		{ u8"mac_greek", static_cast<UINT>(10006u) },
		{ u8"mac_iceland", static_cast<UINT>(10079u) },
		{ u8"mac_turkish", static_cast<UINT>(10081u) },
		{ u8"shift_jis", static_cast<UINT>(932u) },
		{ u8"utf_7", static_cast<UINT>(65000u) },
		{ u8"utf_8", static_cast<UINT>(65001u) },
	};

	static bool GetWindowsCodePage(const std::u8string_view& enc_name, UINT& out_cp) {
		// resolve alias
		std::u8string resolved_name = ResolveEncodingAlias(enc_name);
		// find code page
		YYCC::StringHelper::Lower(resolved_name);
		auto finder = c_WinCPMap.find(resolved_name);
		if (finder == c_WinCPMap.end()) return false;
		// okey, we found it.
		out_cp = finder->second;
		return true;
	}

#else
	
	static const std::map<std::u8string, std::string> c_IconvMap {
		{ u8"ascii", "ASCII" },
		{ u8"big5", "BIG5" },
		{ u8"big5hkscs", "BIG5-HKSCS" },
		{ u8"cp850", "CP850" },
		{ u8"cp862", "CP862" },
		{ u8"cp866", "CP866" },
		{ u8"cp874", "CP874" },
		{ u8"cp932", "CP932" },
		{ u8"cp949", "CP949" },
		{ u8"cp950", "CP950" },
		{ u8"cp1250", "CP1250" },
		{ u8"cp1251", "CP1251" },
		{ u8"cp1252", "CP1252" },
		{ u8"cp1253", "CP1253" },
		{ u8"cp1254", "CP1254" },
		{ u8"cp1255", "CP1255" },
		{ u8"cp1256", "CP1256" },
		{ u8"cp1257", "CP1257" },
		{ u8"cp1258", "CP1258" },
		{ u8"euc_jp", "EUC-JP" },
		{ u8"euc_kr", "EUC-KR" },
		{ u8"gbk", "CP936" },
		{ u8"gb18030", "GB18030" },
		{ u8"hz", "HZ" },
		{ u8"iso2022_jp", "ISO-2022-JP" },
		{ u8"iso2022_jp_1", "ISO-2022-JP-1" },
		{ u8"iso2022_jp_2", "ISO-2022-JP-2" },
		{ u8"iso2022_kr", "ISO-2022-KR" },
		{ u8"latin_1", "ISO-8859-1" },
		{ u8"iso8859_2", "ISO-8859-2" },
		{ u8"iso8859_3", "ISO-8859-3" },
		{ u8"iso8859_4", "ISO-8859-4" },
		{ u8"iso8859_5", "ISO-8859-5" },
		{ u8"iso8859_6", "ISO-8859-6" },
		{ u8"iso8859_7", "ISO-8859-7" },
		{ u8"iso8859_8", "ISO-8859-8" },
		{ u8"iso8859_9", "ISO-8859-9" },
		{ u8"iso8859_10", "ISO-8859-10" },
		{ u8"iso8859_11", "ISO-8859-11" },
		{ u8"iso8859_13", "ISO-8859-13" },
		{ u8"iso8859_14", "ISO-8859-14" },
		{ u8"iso8859_15", "ISO-8859-15" },
		{ u8"iso8859_16", "ISO-8859-16" },
		{ u8"johab", "JOHAB" },
		{ u8"koi8_t", "KOI8-T" },
		{ u8"mac_cyrillic", "MacCyrillic" },
		{ u8"mac_greek", "MacGreek" },
		{ u8"mac_iceland", "MacIceland" },
		{ u8"mac_roman", "MacRoman" },
		{ u8"mac_turkish", "MacTurkish" },
		{ u8"ptcp154", "PT154" },
		{ u8"shift_jis", "SHIFT_JIS" },
		{ u8"utf_32", "UTF-32" },
		{ u8"utf_32_be", "UTF-32BE" },
		{ u8"utf_32_le", "UTF-32LE" },
		{ u8"utf_16", "UTF16" },
		{ u8"utf_16_be", "UTF-16BE" },
		{ u8"utf_16_le", "UTF-16LE" },
		{ u8"utf_7", "UTF-7" },
		{ u8"utf_8", "UTF-8" },
	};

	static bool GetIconvCode(const std::u8string_view& enc_name, std::string& out_code) {
		// resolve alias
		std::u8string resolved_name = ResolveEncodingAlias(enc_name);
		// find code page
		YYCC::StringHelper::Lower(resolved_name);
		auto finder = c_IconvMap.find(resolved_name);
		if (finder == c_IconvMap.end()) return false;
		// okey, we found it.
		out_code = finder->second;
		return true;
	}

#endif

#pragma endregion

#pragma region Internal Functions

#if YYCC_OS == YYCC_OS_WINDOWS

	struct WindowsEncodingToken {
		UINT m_CodePage;
	};

#else

	static constexpr const size_t c_IconvIncUnit = 16u;
	static const iconv_t c_InvalidIconvType = reinterpret_cast<iconv_t>(-1);

	struct IconvEncodingToken {
		IconvEncodingToken(const std::string_view& iconv_code) :
			m_FromUTF8(c_InvalidIconvType), m_ToUTF8(c_InvalidIconvType) {
			// if iconv code is empty, do nothing
			if (iconv_code.empty()) return;
			// setup iconv_t
			this->m_FromUTF8 = iconv_open(code.c_str(), "UTF-8");
			this->m_ToUTF8 = iconv_open("UTF-8", code.c_str());
		}
		~IconvEncodingToken() {
			if (this->m_FromUTF8 != c_InvalidIconvType)
				iconv_close(token_cast->m_FromUTF8);
			if (this->m_ToUTF8 != c_InvalidIconvType)
				iconv_close(token_cast->m_ToUTF8);
		}
		iconv_t m_FromUTF8;
		iconv_t m_ToUTF8;
	};

	// Reference: https://stackoverflow.com/questions/13297458/simple-utf8-utf16-string-conversion-with-iconv
	static bool DoIconv(iconv_t& cd, const std::string_view& str_from, std::string& str_to) {
		char* inbuf = nullptr, * outbuf = nullptr;
		size_t inbytesleft, outbytesleft, nchars, result_len;

		// check empty
		if (str_from.empty()) {
			str_to.clear();
			return true;
		}

		// check iconv descriptor
		if (cd == c_InvalidIconvType) {
			// invalid iconv descriptor
			return false;
		}

		// pre-resize
		str_to.resize(str_from.size() + c_IconvIncUnit);
		// setup some variables
		inbytesleft = str_from.size();
		inbuf = const_cast<char*>(str_from.data());

		outbytesleft = str_to.size();
		outbuf = str_to.data();

		result_len = str_to.size();

		// conv core
		nchars = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
		while (nchars == (size_t)-1 && errno == E2BIG) {
			// record the length has been converted
			size_t len = outbuf - str_to.data();

			// resize for variables
			result_len += c_IconvIncUnit;
			outbytesleft += c_IconvIncUnit;

			// resize for container
			str_to.resize(result_len);

			// assign new outbuf from failed position	
			outbuf = str_to.data() + len;
			nchars = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
		}

		// restore descriptor initial state
		iconv(cd, nullptr, nullptr, nullptr, nullptr);

		// check error
		if (nchars == (size_t)-1) {
			// failed
			return false;
		} else {
			// success
			// resize result to get correct data
			str_to.resize(result_len - outbytesleft);
			return true;
		}

	}

#endif

#pragma endregion

#pragma region Encoding Token

	EncodingToken CreateEncodingToken(const std::u8string_view& enc_name) {
#if YYCC_OS == YYCC_OS_WINDOWS
		// get code page first
		UINT cp = CP_ACP;
		if (!GetWindowsCodePage(enc_name, cp))
			return INVALID_ENCODING_TOKEN;
		// validate code page
		if (!YYCC::WinFctHelper::IsValidCodePage(cp))
			return INVALID_ENCODING_TOKEN;
		// create token and return
		WindowsEncodingToken* token = new WindowsEncodingToken { cp };
		return token;
#else
		// get iconv code first
		std::string code;
		if (!GetIconvCode(enc_name, code))
			return INVALID_ENCODING_TOKEN;
		// create token and set default value
		IconvEncodingToken* token = new IconvEncodingToken(code);
		// check whether token has been initialized correctly
		if (token->m_FromUTF8 == c_InvalidIconvType || token->m_ToUTF8 == c_InvalidIconvType) {
			// failed. free resource and return
			delete token_cast;
			return INVALID_ENCODING_TOKEN;
		}
		// okey, return
		return token;
#endif
	}

	void DestroyEncodingToken(EncodingToken token) {
		// if token is invalid, return directly
		if (token == INVALID_ENCODING_TOKEN) return;

#if YYCC_OS == YYCC_OS_WINDOWS
		WindowsEncodingToken* token_cast = static_cast<WindowsEncodingToken*>(token);
		delete token_cast;
#else
		IconvEncodingToken* token_cast = static_cast<IconvEncodingToken*>(token);
		delete token_cast;
#endif
	}

#pragma endregion

#pragma region Exposed Convertion Functions

	bool ToOrdinary(const std::u8string_view& src, std::string& dst, EncodingToken token) {
		// if token is invalid, return false
		if (token == INVALID_ENCODING_TOKEN) return false;

#if YYCC_OS == YYCC_OS_WINDOWS
		WindowsEncodingToken* token_cast = static_cast<WindowsEncodingToken*>(token);
		return YYCC::EncodingHelper::UTF8ToChar(src, dst, token_cast->m_CodePage);
#else
		IconvEncodingToken* token_cast = static_cast<IconvEncodingToken*>(token);
		return DoIconv(token_cast->FromUTF8, YYCC::EncodingHelper::ToOrdinaryView(src), dst);
#endif
	}
	bool ToOrdinary(const char8_t* src, std::string& dst, EncodingToken token) {
		if (src == nullptr) return false;
		return ToOrdinary(std::u8string_view(src), dst, token);
	}
	std::string ToOrdinary(const std::u8string_view& src, EncodingToken token) {
		std::string ret;
		if (!ToOrdinary(src, ret, token)) ret.clear();
		return ret;
	}
	std::string ToOrdinary(const char8_t* src, EncodingToken token) {
		std::string ret;
		if (!ToOrdinary(src, ret, token)) ret.clear();
		return ret;
	}

	bool ToUTF8(const std::string_view& src, std::u8string& dst, EncodingToken token) {
		// if token is invalid, return false
		if (token == INVALID_ENCODING_TOKEN) return false;

#if YYCC_OS == YYCC_OS_WINDOWS
		WindowsEncodingToken* token_cast = static_cast<WindowsEncodingToken*>(token);
		return YYCC::EncodingHelper::CharToUTF8(src, dst, token_cast->m_CodePage);
#else
		IconvEncodingToken* token_cast = static_cast<IconvEncodingToken*>(token);
		std::string dst_cache;
		bool ret = DoIconv(token_cast->ToUTF8, src, dst_cache);
		if (ret) dst = YYCC::EncodingHelper::ToUTF8(dst_cache);
		return ret;
#endif
	}
	bool ToUTF8(const char* src, std::u8string& dst, EncodingToken token) {
		if (src == nullptr) return false;
		return ToUTF8(std::string_view(src), dst, token);
	}
	std::u8string ToUTF8(const std::string_view& src, EncodingToken token) {
		std::u8string ret;
		if (!ToUTF8(src, ret, token)) ret.clear();
		return ret;
	}
	std::u8string ToUTF8(const char* src, EncodingToken token) {
		std::u8string ret;
		if (!ToUTF8(src, ret, token)) ret.clear();
		return ret;
	}

#pragma endregion

}

