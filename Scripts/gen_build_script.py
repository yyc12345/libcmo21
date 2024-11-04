import os
import argparse
import jinja2

def get_root_directory() -> str:
    return os.path.dirname(os.path.dirname(__file__))

class ScriptSettings:
    m_BuildDoc: bool

    def __init__(self, build_doc: bool):
        self.m_BuildDoc = build_doc

class TemplateRender:
    m_Loader: jinja2.BaseLoader
    m_Environment: jinja2.Environment

    m_WinTemplate: jinja2.Template
    m_LinuxTemplate: jinja2.Template

    m_Settings: ScriptSettings

    def __init__(self, settings: ScriptSettings) -> None:
        self.m_Loader = jinja2.FileSystemLoader(self.__get_dir())
        self.m_Environment = jinja2.Environment(loader=self.m_Loader)

        self.m_WinTemplate = self.m_Environment.get_template('win_build.template.bat')
        self.m_LinuxTemplate = self.m_Environment.get_template('linux_build.template.sh')

        self.m_Settings = settings

    def __get_dir(self) -> str:
        return os.path.dirname(__file__)

    def __render(self, template: jinja2.Template, dest_file: str, is_win: bool) -> None:
        with open(os.path.join(self.__get_dir(), dest_file), 'w', encoding='utf-8') as f:
            f.write(template.render(
                repo_root_dir = os.path.dirname(self.__get_dir()),
                build_doc = self.m_Settings.m_BuildDoc
            ))

    def render_win_script(self) -> None:
        self.__render(self.m_WinTemplate, 'win_build.bat', True)
    
    def render_linux_script(self) -> None:
        self.__render(self.m_LinuxTemplate, 'linux_build.sh', False)
    
if __name__ == '__main__':
    # parse argument
    parser = argparse.ArgumentParser(
        prog='LibCmo Windows Build Script',
        description='LibCmo Windows Build Script'
    )
    parser.add_argument(
        '-d', '--build-doc',
        action='store_true', dest='build_doc',
        help='Build LibCmo without documentation.'
    )
    args = parser.parse_args()

    # build settings
    settings = ScriptSettings(args.build_doc)
    # build template render and render result
    render = TemplateRender(settings)
    render.render_win_script()
    render.render_linux_script()
    