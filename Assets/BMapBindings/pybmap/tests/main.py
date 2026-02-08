import os
from pathlib import Path
from tempfile import TemporaryDirectory
import pybmap.bmap_wrapper as bmap
import cli
import testsuits

def main() -> None:
    # Parse arguments
    try:
        cliopts = cli.Cli()
    except cli.CliException as e:
        print(f'Can not launch test. Reason: {e}')
        return
    
    # Check BMap status.
    # if True:
    #     print('Fail to initialize native BMap.')
    #     return

    # Waiting debugger
    input(f'Python PID is {os.getpid()}. Waiting for debugger, press any key to continue...')

    # Start testbench
    with TemporaryDirectory() as tempdir:
        file_name = str(cliopts.file_name)
        temp_folder = str(tempdir)
        texture_folder = str(cliopts.ballance_dir / 'Textures')
        encodings = cliopts.encodings
        with bmap.BMFileReader(file_name, temp_folder, texture_folder, encodings) as reader:
            testsuits.TestCommon.test(reader)
            testsuits.TestEq.test(reader)

if __name__ == '__main__':
    main()
