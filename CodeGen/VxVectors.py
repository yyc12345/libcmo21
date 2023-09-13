
def GetTmplDecl(svars: tuple[str]) -> str:
    return f'float {", ".join(svars)};'

def GetTmplCtor1(sname: str, svars: tuple[str]) -> str:
    return f'{sname}() : {", ".join(map(lambda x: f"{x}(0.0f)", svars))} {{}}'

def GetTmplCtor2(sname: str, svars: tuple[str]) -> str:
    return f'{sname}({", ".join(map(lambda x: f"float _{x}", svars))}) : {", ".join(map(lambda x: f"{x}(_{x})", svars))} {{}}'

def GetTmplCopyCtor(sname: str, svars: tuple[str]) -> str:
    return f'{sname}(const {sname}& rhs) : {", ".join(map(lambda x: f"{x}(rhs.{x})", svars))} {{}}'

def GetTmplMoveCtor(sname: str, svars: tuple[str]) -> str:
    return f'{sname}({sname}&& rhs) : {", ".join(map(lambda x: f"{x}(rhs.{x})", svars))} {{}}'

def GetTmplOperAssignCopy(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\t{sname}& operator=(const {sname}& rhs) {{
\t\t{sp.join(map(lambda x: f'{x} = rhs.{x};', svars))}
\t\treturn *this;
\t}}"""

def GetTmplOperAssignMove(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\t{sname}& operator=({sname}&& rhs) {{
\t\t{sp.join(map(lambda x: f'{x} = rhs.{x};', svars))}
\t\treturn *this;
\t}}"""

def GetTmplOperOffset(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t\t'
    return f"""\tfloat& operator[](size_t i) {{
\t\tswitch (i) {{
\t\t\t{sp.join(map(lambda x: f'case {x}: return {svars[x]};', range(len(svars))))}
\t\t\tdefault: return {svars[0]};
\t\t}}
\t}}"""

def GetTmplOperAddMinus(sname: str, svars: tuple[str], oper: str) -> str:
    sp: str = '\n\t\t'
    return f"""\t{sname}& operator{oper}=(const {sname}& rhs) {{
\t\t{sp.join(map(lambda x: f'{x} {oper}= rhs.{x};', svars))}
\t\treturn *this;
\t}}
\tfriend {sname} operator{oper}(const {sname}& lhs, const {sname}& rhs) {{
\t\treturn {sname}({', '.join(map(lambda x: f'lhs.{x} {oper} rhs.{x}', svars))});
\t}}"""

def GetTmplOperMul(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\t{sname}& operator*=(float rhs) {{
\t\t{sp.join(map(lambda x: f'{x} *= rhs;', svars))}
\t\treturn *this;
\t}}
\tfriend {sname} operator*(const {sname}& lhs, float rhs) {{
\t\treturn {sname}({', '.join(map(lambda x: f'lhs.{x} * rhs', svars))});
\t}}
\tfriend {sname} operator*(float lhs, const {sname}& rhs) {{
\t\treturn {sname}({', '.join(map(lambda x: f'lhs * rhs.{x}', svars))});
\t}}"""

def GetTmplOperDiv(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\t{sname}& operator/=(float rhs) {{
\t\tif (rhs == 0.0f) return *this;
\t\t{sp.join(map(lambda x: f'{x} /= rhs;', svars))}
\t\treturn *this;
\t}}
\tfriend {sname} operator/(const {sname}& lhs, float rhs) {{
\t\tif (rhs == 0.0f) return {sname}({', '.join(map(lambda x: '0.0f', range(len(svars))))});
\t\treturn {sname}({', '.join(map(lambda x: f'lhs.{x} / rhs', svars))});
\t}}"""

def GetTmplOperEqual(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\tbool operator==(const {sname}& rhs) const {{
\t\treturn ({' && '.join(map(lambda x: f'{x} == rhs.{x}', svars))});
\t}}
\tbool operator!=(const {sname}& rhs) const {{
\t\treturn !(*this == rhs);
\t}}"""

def GetTmplVector(sname: str, svars: tuple[str]) -> str:
    return f"""
struct {sname} {{
\t{GetTmplDecl(svars)}
\t{GetTmplCtor1(sname, svars)}
\t{GetTmplCtor2(sname, svars)}
\tLIBCMO_DEFAULT_COPY_MOVE({sname});
{GetTmplOperOffset(sname, svars)}
{GetTmplOperAddMinus(sname, svars, '+')}
{GetTmplOperAddMinus(sname, svars, '-')}
{GetTmplOperMul(sname, svars)}
{GetTmplOperDiv(sname, svars)}
{GetTmplOperEqual(sname, svars)}
}};
"""

def GetTmplOthers(sname: str, svars: tuple[str]) -> str:
    return f"""
struct {sname} {{
\t{GetTmplDecl(svars)}
\t{GetTmplCtor1(sname, svars)} // set your custom init.
\t{GetTmplCtor2(sname, svars)}
\tLIBCMO_DEFAULT_COPY_MOVE({sname});
{GetTmplOperOffset(sname, svars)}
{GetTmplOperEqual(sname, svars)}
}};
"""

# use LIBCMO_DEFAULT_COPY_MOVE instead of these outputs.
#\t{GetTmplCopyCtor(sname, svars)}
#\t{GetTmplMoveCtor(sname, svars)}
#{GetTmplOperAssignCopy(sname, svars)}
#{GetTmplOperAssignMove(sname, svars)}

if __name__ == '__main__':
    with open('VxTypes.hpp', 'w', encoding='utf-8') as fs:
        fs.write(GetTmplVector('VxVector2', ('x', 'y', )))
        fs.write(GetTmplVector('VxVector3', ('x', 'y', 'z', )))
        fs.write(GetTmplVector('VxVector4', ('x', 'y', 'z', 'w', )))
        fs.write(GetTmplOthers('VxQuaternion', ('x', 'y', 'z', 'w', )))
        fs.write(GetTmplOthers('VxColor', ('r', 'g', 'b', 'a', )))
