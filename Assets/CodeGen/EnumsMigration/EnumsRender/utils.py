import os
import enum
from pathlib import Path


class CKParts(enum.StrEnum):
    CK2 = "CK2"
    VxMath = "VxMath"


def _get_root_directory() -> Path:
    enums_migration_root = os.environ.get("ENUMS_MIGRATION_ROOT", None)
    if enums_migration_root is None:
        return Path(__file__).resolve().parent.parent
    else:
        return Path(enums_migration_root).resolve()


def get_input_file_path(filename: str) -> Path:
    return _get_root_directory() / "Intermediate" / filename


def get_output_file_path(filename: str) -> Path:
    return _get_root_directory() / "Output" / filename


def get_template_directory() -> Path:
    return Path(__file__).resolve().parent / "templates"
