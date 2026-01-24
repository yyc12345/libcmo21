import os
import io
import typing
import jinja2

g_HppTemplateFile: str = 'VxTypes.hpp.jinja'
g_CppTemplateFile: str = 'VxTypes.cpp.jinja'

g_ResultHppFile: str = 'VxTypes.hpp'
g_ResultCppFile: str = 'VxTypes.cpp'

def get_root_directory() -> str:
    return os.path.dirname(__file__)

class TemplateRender:
    m_Loader: jinja2.BaseLoader
    m_Environment: jinja2.Environment

    m_HppTemplate: jinja2.Template
    m_CppTemplate: jinja2.Template

    m_OutputHpp: io.TextIOWrapper
    m_OutputCpp: io.TextIOWrapper

    def __init__(self, output_hpp_path: str, output_cpp_path: str) -> None:
        self.m_Loader = jinja2.FileSystemLoader(get_root_directory())
        self.m_Environment = jinja2.Environment(loader=self.m_Loader)

        self.m_HppTemplate = self.m_Environment.get_template(g_HppTemplateFile)
        self.m_CppTemplate = self.m_Environment.get_template(g_CppTemplateFile)

        self.m_OutputHpp = open(os.path.join(get_root_directory(), output_hpp_path), 'w', encoding='utf-8')
        self.m_OutputCpp = open(os.path.join(get_root_directory(), output_cpp_path), 'w', encoding='utf-8')

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        self.m_OutputHpp.close()
        self.m_OutputCpp.close()
    
    def __render(self, sname: str, is_vector: bool, svars: tuple[str]) -> None:
        template_argument: dict[str, typing.Any] = {
            'sname': sname,
            'is_vector': is_vector,
            'svars': svars
        }
        self.m_OutputHpp.write(self.m_HppTemplate.render(**template_argument))
        self.m_OutputCpp.write(self.m_CppTemplate.render(**template_argument))

    def render_vector(self, sname: str, svars: tuple[str]) -> None:
        self.__render(sname, True, svars)

    def render_others(self, sname: str, svars: tuple[str]) -> None:
        self.__render(sname, False, svars)

if __name__ == '__main__':
    with TemplateRender(g_ResultHppFile, g_ResultCppFile) as render:
        render.render_vector('VxVector2', ('x', 'y', ))
        render.render_vector('VxVector3', ('x', 'y', 'z', ))
        render.render_vector('VxVector4', ('x', 'y', 'z', 'w', ))
        render.render_others('VxQuaternion', ('x', 'y', 'z', 'w', ))
        render.render_others('VxColor', ('r', 'g', 'b', 'a', ))
