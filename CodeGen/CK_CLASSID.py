class CKClass():
    def __init__(self, name: str, value: int):
        self.name: str = name
        self.value: int = value
        self.parents: tuple[str] = None

def GetLevel(strl: str):
    counter = 0
    for c in strl:
        if c == '\t':
            counter += 1
        else:
            break
    return counter

def BuildClass(strl: str) -> CKClass:
    strl = strl.replace('#define', '\t').replace(' ', '\t').strip()
    sp = strl.split('\t')
    return CKClass(sp[0], sp[-1])

def GetParents(ls: list[CKClass]) -> tuple[str]:
    return tuple(
        map(lambda x: x.name, ls)
    )

full_classes = []
with open('src/CK_CLASSID.txt', 'r', encoding='utf-8') as fr:
    level = 0
    node_stack: list[CKClass] = [None]

    while True:
        ln = fr.readline()
        if ln == '':
            break
        if ln.strip() == '':
            continue
        ln = ln.strip('\n')

        new_item = BuildClass(ln)
        full_classes.append(new_item)

        this_level = GetLevel(ln)
        if this_level > level:
            # level up
            level += 1
            node_stack.append(new_item)
            new_item.parents = GetParents(node_stack)
        elif this_level == level:
            node_stack.pop()
            node_stack.append(new_item)
            new_item.parents = GetParents(node_stack)
        elif this_level < level:
            for i in range(level - this_level + 1):
                node_stack.pop()
            level = this_level

            node_stack.append(new_item)
            new_item.parents = GetParents(node_stack)


    fr.close()

with open('dest/CK_CLASSID.txt', 'w', encoding='utf-8') as fw:
    for item in full_classes:
        fw.write('{{ LibCmo::CK_CLASSID::{}, {{{}}} }},\n'.format(
            item.parents[-1], 
            ', '.join(
                map(lambda x: '"' + x + '"', item.parents)
            )
        ))

    fw.write('\n')

    for item in full_classes:
        fw.write(f'{item.name} = {item.value},\n')
    fw.close()
