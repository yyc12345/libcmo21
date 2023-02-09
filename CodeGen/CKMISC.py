def RemoveAnnotation(strl: str) -> str:
    idx = strl.find("//")
    if idx == -1: return strl.strip()

    return strl[:idx].strip()

def Underline2Camel(orig: str) -> str:
    return ''.join(map(
        lambda strl: strl[0].upper() + strl[1:].lower(),
        orig.split('_')
    ))

class EnumEntry():
    def __init__(self, name: str, val: str):
        self.name: str = name
        self.val: str = val
    def __repr__(self) -> str:
        return f'<{self.name}: {self.val}>'
    

class EnumBody():
    def __init__(self, name: str, is_flag: bool):
        self.name: str = name
        self.camel_name: str = Underline2Camel(name)
        self.is_flag: bool = is_flag
        self.entries: list[EnumEntry] = []
    def __repr__(self) -> str:
        return self.entries.__repr__()

full_enum: list[EnumBody] = []
current_body: EnumEntry = None
with open('src/CKMISC.txt', 'r', encoding='utf-8') as fr:
    while True:
        ln = fr.readline()
        if ln == '':
            break
        ln = RemoveAnnotation(ln)
        if ln == '':
            continue

        if ln.startswith('enum'):
            ln = ln[len('enum'):].strip()

            is_flag = ln[0] == '!'
            name = ln[1:] if is_flag else ln

            if current_body:
                full_enum.append(current_body)
            current_body = EnumBody(name, is_flag)

        else:
            sp = ln.replace(',', '').split('=')
            if len(sp) == 1:
                entry = EnumEntry(sp[0].strip(), '')
            else:
                entry = EnumEntry(sp[0].strip(), sp[-1].strip())
            current_body.entries.append(entry)

    fr.close()
if current_body:
    full_enum.append(current_body)

with open('dest/CKMISC.txt', 'w', encoding='utf-8') as fw:
    # write define
    for item in full_enum:
        fw.write('enum class {} : int32_t {{\n'.format(item.name))

        fw.write(',\n'.join(map(
            lambda x: x.name if x.val == '' else (x.name + ' = ' + x.val),
            item.entries
        )))
        fw.write('\n};\n')

    fw.write('\n')

    # write decl
    fw.write('namespace EnumDesc {\n')
    for item in full_enum:
        fw.write('extern const EnumDescPairArray<LibCmo::{}> {};\n'.format(
            item.name, item.name
        ))
    fw.write('}')

    fw.write('\n')

    # write vector
    fw.write('namespace EnumDesc {\n')
    for item in full_enum:
        fw.write('const EnumDescPairArray<LibCmo::{}> {} {{\n'.format(
            item.name, item.name
        ))
        fw.write(',\n'.join(map(
            lambda x: '{{ LibCmo::{}::{}, "{}" }}'.format(item.name, x.name, x.name),
            item.entries
        )))
        fw.write('\n};\n')
    fw.write('}')

    fw.write('\n')


    fw.close()
