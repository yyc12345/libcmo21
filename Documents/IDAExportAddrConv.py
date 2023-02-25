def Conv(strl: str):
    if len(strl) & 3 != 0:
        print('Invalid length!')
        return

    idaBytes = tuple(strl[i:i+2] for i in range(0, len(strl), 2))
    for i in range(0, len(idaBytes), 4):
        print(idaBytes[i+3] + idaBytes[i+2] + idaBytes[i+1] + idaBytes[i])

while True:
    Conv(input("data> "))

