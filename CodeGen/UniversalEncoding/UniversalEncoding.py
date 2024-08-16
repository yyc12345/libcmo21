import typing
import io

class LanguageToken:
    m_Name: str
    m_Alias: tuple[str, ...]
    m_CodePage: str | None
    m_IconvCode: str | None

    def __init__(self, name: str, alias: typing.Iterator[str], code_page: str, iconv_code: str):
        self.m_Name = name.lower()
        self.m_Alias = tuple(map(lambda x: x.lower(), alias))
        self.m_CodePage = None if code_page == '' else code_page
        self.m_IconvCode = None if iconv_code == '' else iconv_code

def extract_data(fs: io.TextIOWrapper) -> tuple[str, ...]:
    # remove first line to remove table header
    return fs.readlines()[1:]

def extract_token(csv_data: tuple[str, ...]) -> tuple[LanguageToken, ...]:
    ret: list[LanguageToken] = []
    for line in csv_data:
        line = line.strip('\n')
        line_sp = line.split('\t')
        alias_sp = filter(lambda x: x != '', map(lambda x: x.strip(), line_sp[1].split(',')))
        ret.append(LanguageToken(line_sp[0], alias_sp, line_sp[2], line_sp[3]))
    return tuple(ret)

def write_alias_map(fs: io.TextIOWrapper, data: tuple[LanguageToken, ...]) -> None:
    fs.write('static const std::map<std::u8string, std::u8string> c_AliasMap {\n')
    for i in data:
        for j in i.m_Alias:
            fs.write(f'\t{{ u8"{j}", u8"{i.m_Name}" }},\n')
    fs.write('};\n')

def write_win_cp_map(fs: io.TextIOWrapper, data: tuple[LanguageToken, ...]) -> None:
    fs.write('static const std::map<std::u8string, UINT> c_WinCPMap {\n')
    for i in data:
        if i.m_CodePage is not None:
            fs.write(f'\t{{ u8"{i.m_Name}", static_cast<UINT>({i.m_CodePage}u) }},\n')
    fs.write('};\n')

def write_iconv_map(fs: io.TextIOWrapper, data: tuple[LanguageToken, ...]) -> None:
    fs.write('static const std::map<std::u8string, std::string> c_IconvMap {\n')
    for i in data:
        if i.m_IconvCode is not None:
            fs.write(f'\t{{ u8"{i.m_Name}", "{i.m_IconvCode}" }},\n')
    fs.write('};\n')

if __name__ == '__main__':
    with open('EncodingTable.csv', 'r', encoding='utf-8') as fr:
        with open('UEncodingTable.cpp', 'w', encoding='utf-8') as fw:
            data = extract_data(fr)
            token = extract_token(data)
            write_alias_map(fw, token)
            write_win_cp_map(fw, token)
            write_iconv_map(fw, token)
