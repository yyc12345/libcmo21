import os
from pathlib import Path


def _get_root_directory() -> Path:
    bmap_binder_root = os.environ.get("BMAP_BINDER_ROOT", None)
    if bmap_binder_root is None:
        return Path(__file__).resolve().parent.parent
    else:
        return Path(bmap_binder_root).resolve()


def get_input_file_path(filename: str) -> Path:
    return _get_root_directory() / "Analyzed" / filename


def get_output_file_path(filename: str) -> Path:
    return _get_root_directory() / "Output" / filename


def get_template_directory() -> Path:
    return Path(__file__).resolve().parent / "templates"
