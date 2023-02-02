
def OutputSizeHumanReadable(storage_size: int):
    probe = storage_size

    # check Bytes
    if (probe >> 10) == 0:
        return f"{storage_size:d}Bytes"
    probe >>= 10

    # check KiB
    if (probe >> 10) == 0:
        return f"{(storage_size / (1 << 10)):.2f}KiB"
    probe >>= 10

    # check MiB
    if (probe >> 10) == 0:
        return f"{(storage_size / (1 << 20)):.2f}MiB"
    probe >>= 10

    # otherwise GiB
    return f"{(storage_size / (1 << 30)):.2f}GiB"

def BcdCodeToDecCode(bcd_num: int):
    result = 0
    pow = 1

    while bcd_num != 0:
        result += (bcd_num & 0xf) * pow
        bcd_num >>= 4
        pow *= 10

    return result
