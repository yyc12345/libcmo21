import os

def GetTmplDecl(svars: tuple[str]) -> str:
    return f'CKFLOAT {", ".join(svars)};'

def GetTmplCtor1(sname: str, svars: tuple[str]) -> str:
    return f'{sname}() : {", ".join(map(lambda x: f"{x}(0.0f)", svars))} {{}}'

def GetTmplCtor2(sname: str, svars: tuple[str]) -> str:
    return f'{sname}({", ".join(map(lambda x: f"CKFLOAT _{x}", svars))}) : {", ".join(map(lambda x: f"{x}(_{x})", svars))} {{}}'

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
    return f"""\tCKFLOAT& operator[](size_t i) {{
\t\tswitch (i) {{
\t\t\t{sp.join(map(lambda x: f'case {x}: return {svars[x]};', range(len(svars))))}
\t\t\tdefault: throw LogicException("Invalid index for {sname}::operator[].");
\t\t}}
\t}}
\tconst CKFLOAT& operator[](size_t i) const {{
\t\tswitch (i) {{
\t\t\t{sp.join(map(lambda x: f'case {x}: return {svars[x]};', range(len(svars))))}
\t\t\tdefault: throw LogicException("Invalid index for {sname}::operator[].");
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
    return f"""\t{sname}& operator*=(CKFLOAT rhs) {{
\t\t{sp.join(map(lambda x: f'{x} *= rhs;', svars))}
\t\treturn *this;
\t}}
\tfriend {sname} operator*(const {sname}& lhs, CKFLOAT rhs) {{
\t\treturn {sname}({', '.join(map(lambda x: f'lhs.{x} * rhs', svars))});
\t}}
\tfriend {sname} operator*(CKFLOAT lhs, const {sname}& rhs) {{
\t\treturn {sname}({', '.join(map(lambda x: f'lhs * rhs.{x}', svars))});
\t}}
\tfriend CKFLOAT operator*(const {sname}& lhs, const {sname}& rhs) {{
\t\treturn ({' + '.join(map(lambda x: f'lhs.{x} * rhs.{x}', svars))});
\t}}"""

def GetTmplOperDiv(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\t{sname}& operator/=(CKFLOAT rhs) {{
\t\tif (rhs == 0.0f) return *this;
\t\t{sp.join(map(lambda x: f'{x} /= rhs;', svars))}
\t\treturn *this;
\t}}
\tfriend {sname} operator/(const {sname}& lhs, CKFLOAT rhs) {{
\t\tif (rhs == 0.0f) return {sname}({', '.join(map(lambda x: '0.0f', range(len(svars))))});
\t\treturn {sname}({', '.join(map(lambda x: f'lhs.{x} / rhs', svars))});
\t}}"""

def GetTmplOperEqual(sname: str, svars: tuple[str]) -> str:
    return f"""\tbool operator==(const {sname}& rhs) const {{
\t\treturn ({' && '.join(map(lambda x: f'{x} == rhs.{x}', svars))});
\t}}"""

def GetTmplOperSpaceship(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\tauto operator<=>(const {sname}& rhs) const {{
\t\t{sp.join(map(lambda x: f'if (auto cmp = {x} <=> rhs.{x}; cmp != 0) return cmp;', svars[:-1]))}
\t\treturn {svars[-1]} <=> rhs.{svars[-1]};
\t}}"""

def GetTmplLength(sname: str, svars: tuple[str]) -> str:
    return f"""\tCKFLOAT SquaredLength() const {{
\t\treturn ({' + '.join(map(lambda x: f'{x} * {x}', svars))});
\t}}
\tCKFLOAT Length() const {{
\t\treturn std::sqrt(SquaredLength());
\t}}"""

def GetTmplNormalize(sname: str, svars: tuple[str]) -> str:
    sp: str = '\n\t\t'
    return f"""\tvoid Normalized() {{
\t\tCKFLOAT len = Length();
\t\tif (len == 0.0f) return;
\t\t{sp.join(map(lambda x: f'{x} /= len;', svars))}
\t}}
\t{sname} Normalize() const {{
\t\tCKFLOAT len = Length();
\t\tif (len == 0.0f) return {sname}();
\t\treturn {sname}({', '.join(map(lambda x: f'{x} / len', svars))});
\t}}"""

def GetTmplVector(sname: str, svars: tuple[str]) -> str:
    return f"""
struct {sname} {{
\t{GetTmplDecl(svars)}
\t{GetTmplCtor1(sname, svars)}
\t{GetTmplCtor2(sname, svars)}
\tYYCC_DEF_CLS_COPY_MOVE({sname});
{GetTmplOperOffset(sname, svars)}
{GetTmplOperAddMinus(sname, svars, '+')}
{GetTmplOperAddMinus(sname, svars, '-')}
{GetTmplOperMul(sname, svars)}
{GetTmplOperDiv(sname, svars)}
{GetTmplOperEqual(sname, svars)}
{GetTmplLength(sname, svars)}
{GetTmplNormalize(sname, svars)}
}};
"""

def GetTmplOthers(sname: str, svars: tuple[str]) -> str:
    return f"""
struct {sname} {{
\t{GetTmplDecl(svars)}
\t{GetTmplCtor1(sname, svars)} // set your custom init.
\t{GetTmplCtor2(sname, svars)}
\tYYCC_DEF_CLS_COPY_MOVE({sname});
{GetTmplOperOffset(sname, svars)}
{GetTmplOperEqual(sname, svars)}
}};
"""

# use YYCC_DEF_CLS_COPY_MOVE instead of these outputs.
#\t{GetTmplCopyCtor(sname, svars)}
#\t{GetTmplMoveCtor(sname, svars)}
#{GetTmplOperAssignCopy(sname, svars)}
#{GetTmplOperAssignMove(sname, svars)}

if __name__ == '__main__':
    # get file path
    self_path: str = os.path.dirname(__file__)
    cpp_file: str = os.path.join(self_path, 'VxTypes.hpp')
    # generate files
    with open(cpp_file, 'w', encoding='utf-8') as fs:
        fs.write(GetTmplVector('VxVector2', ('x', 'y', )))
        fs.write(GetTmplVector('VxVector3', ('x', 'y', 'z', )))
        fs.write(GetTmplVector('VxVector4', ('x', 'y', 'z', 'w', )))
        fs.write(GetTmplOthers('VxQuaternion', ('x', 'y', 'z', 'w', )))
        fs.write(GetTmplOthers('VxColor', ('r', 'g', 'b', 'a', )))
