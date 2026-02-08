import os
from pathlib import Path

class CliException(Exception):
    """Error occurs when parsing test arguments."""
    pass

class Cli:
    
    file_name: Path
    """
    The path to the map for loading.
    """
    ballance_dir: Path
    """
    The path to the Ballance directory for finding textures
    """
    encodings: tuple[str, ...]
    """
    The name of encodings used by BMap for loading map.
    """

    def __init__(self) -> None:
        file_name = os.environ.get('BMAP_FILE_NAME', None)
        if file_name is None:
            raise CliException('You must specify BMAP_FILE_NAME environment variable before running this test.')
        else:
            self.file_name = Path(file_name).resolve()

        ballance_dir = os.environ.get('BMAP_BALLANCE_DIR', None)
        if ballance_dir is None:
            raise CliException('You must specify BMAP_BALLANCE_DIR environment variable before running this test.')
        else:
            self.ballance_dir = Path(ballance_dir).resolve()

        encodings = os.environ.get('BMAP_ENCODINGS', None)
        if encodings is None:
            raise CliException('You must specify BMAP_ENCODINGS environment variable before running this test.')
        else:
            self.encodings = tuple(encodings.split(','))
