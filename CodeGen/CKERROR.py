full_error = []
annotation = ''
with open('src/CKERROR.txt', 'r', encoding='utf-8') as fr:
    while True:
        ln = fr.readline()
        if ln == '':
            break
        ln.strip()
        if ln == '':
            continue

        if ln.startswith('#define'):
            sp = ln[len('#define'):].strip().split(' ')
            # name, value, description
            full_error.append((sp[0], sp[-1], annotation))
            annotation = ''
        elif ln.startswith('//'):
            annotation = ln[len('//'):].strip()

    fr.close()

with open('dest/CKERROR.txt', 'w', encoding='utf-8') as fw:
    for item in full_error:
        fw.write('{{ LibCmo::CKERROR::{}, {{"{}", "{}"}} }},\n'.format(
            item[0], 
            item[0],
            item[-1])
        )

    fw.write('\n')

    for item in full_error:
        fw.write(f'{item[0]} = {item[1]},\n')
    fw.close()
