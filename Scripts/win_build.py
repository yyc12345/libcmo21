import subprocess
import os
import shutil
import argparse

def get_root_directory() -> str:
    return os.path.dirname(os.path.dirname(__file__))

def execute_cmd(prog: str, args: tuple[str, ...], cwd: str) -> None:
    # find program first
    found_prog = shutil.which(prog)
    if found_prog is None:
        raise RuntimeError(f'Fail to find program {prog}')
    # run command
    subprocess.run(
        list((found_prog, ) + args),      # program + arguments
        stdin=subprocess.PIPE,      # redirect
        stdout=subprocess.PIPE,     # redirect
        stderr=subprocess.STDOUT,   # stderr use the same output with stdout
        cwd=cwd,                    # work directory
        shell=True,                 # enable shell feature
        check=True,                 # if program failed, raise exception and exit
    )

def build(no_doc: bool) -> None:
    # create directory
    root_dir: str = get_root_directory()
    os.makedirs(os.path.join(root_dir, 'Bin', 'build'))
    os.makedirs(os.path.join(root_dir, 'Bin', 'install'))

    # build project
    args = [
        ''
    ]

if __name__ == '__main__':
    # parse argument
    parser = argparse.ArgumentParser(
        prog='LibCmo Windows Build Script',
        description='LibCmo Windows Build Script'
    )
    parser.add_argument(
        '-d', '--no-doc',
        action='store_true', dest='no_doc',
        help='Build LibCmo without documentation.'
    )
    args = parser.parse_args()
    build(args.no_doc)
