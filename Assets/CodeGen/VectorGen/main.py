import typing
import jinja2
from pathlib import Path

HPP_TEMPLATE_FILE: str = 'VxTypes.hpp.jinja'
CPP_TEMPLATE_FILE: str = 'VxTypes.cpp.jinja'

HPP_RENDER_FILE: str = 'VxTypes.hpp'
CPP_RENDER_FILE: str = 'VxTypes.cpp'

def get_root_directory() -> Path:
    return Path(__file__).resolve().parent

class TemplateRender:
    __loader: jinja2.BaseLoader
    __environment: jinja2.Environment

    __hpp_template: jinja2.Template
    __cpp_template: jinja2.Template

    __render_hpp: typing.TextIO
    __render_cpp: typing.TextIO

    def __init__(self, output_hpp_path: str, output_cpp_path: str) -> None:
        self.__loader = jinja2.FileSystemLoader(get_root_directory())
        self.__environment = jinja2.Environment(loader=self.__loader)

        self.__hpp_template = self.__environment.get_template(HPP_TEMPLATE_FILE)
        self.__cpp_template = self.__environment.get_template(CPP_TEMPLATE_FILE)

        self.__render_hpp = open(get_root_directory() / output_hpp_path, 'w', encoding='utf-8')
        self.__render_cpp = open(get_root_directory() / output_cpp_path, 'w', encoding='utf-8')

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        self.__render_hpp.close()
        self.__render_cpp.close()
    
    def __render(self, sname: str, is_vector: bool, svars: tuple[str, ...]) -> None:
        template_argument: dict[str, typing.Any] = {
            'sname': sname,
            'is_vector': is_vector,
            'svars': svars
        }
        self.__render_hpp.write(self.__hpp_template.render(**template_argument))
        self.__render_cpp.write(self.__cpp_template.render(**template_argument))

    def render_vector(self, sname: str, svars: tuple[str, ...]) -> None:
        self.__render(sname, True, svars)

    def render_others(self, sname: str, svars: tuple[str, ...]) -> None:
        self.__render(sname, False, svars)

if __name__ == '__main__':
    with TemplateRender(HPP_RENDER_FILE, CPP_RENDER_FILE) as render:
        render.render_vector('VxVector2', ('x', 'y', ))
        render.render_vector('VxVector3', ('x', 'y', 'z', ))
        render.render_vector('VxVector4', ('x', 'y', 'z', 'w', ))
        render.render_others('VxQuaternion', ('x', 'y', 'z', 'w', ))
        render.render_others('VxColor', ('r', 'g', 'b', 'a', ))
    print('Done')
