import os
import re
from pathlib import Path

# region: Path Getter


def _get_libcmo21_repo_directory() -> Path:
    repo_root = os.environ.get("LIBCMO21_REPO_ROOT", None)
    if repo_root is None:
        return Path(__file__).resolve().parent.parent.parent.parent.parent
    else:
        return Path(repo_root).resolve()


def get_input_file_path() -> Path:
    return _get_libcmo21_repo_directory() / "BMap" / "BMap" / "BMExports.hpp"


def _get_bmap_binder_directory() -> Path:
    bmap_binder_root = os.environ.get("BMAP_BINDER_ROOT", None)
    if bmap_binder_root is None:
        return Path(__file__).resolve().parent.parent
    else:
        return Path(bmap_binder_root).resolve()


def get_output_file_path() -> Path:
    return _get_bmap_binder_directory() / "Extracted" / "BMExports.hpp"


# endregion

# region: Extractor

# We should not only match BMAP_EXPORT,
# because it may match the defination of BMAP_EXPORT.
# So we add a bool at head because all BMap functions return bool.
EXPFCTS_PATTERN: re.Pattern = re.compile(
    "^BMAP_EXPORT[ \\t]+bool[ \\t]+[^;]+;", re.MULTILINE
)


def extract_expfcts(infile: Path, outfile: Path) -> None:
    with open(infile, "r", encoding="utf-8") as fin:
        # read full text
        fulltext: str = fin.read()
        # do findall and write into file
        with open(outfile, "w", encoding="utf-8") as fout:
            for item in EXPFCTS_PATTERN.findall(fulltext):
                fout.write(item)
                fout.write("\n")


# endregion


def main():
    extract_expfcts(get_input_file_path(), get_output_file_path())
    print("Done")


if __name__ == "__main__":
    main()
