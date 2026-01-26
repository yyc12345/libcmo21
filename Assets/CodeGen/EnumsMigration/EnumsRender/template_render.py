import jinja2
import typing
from json_loader import BEnumCollection, BEnum
from utils import CKParts
import utils

class RenderUtils:
    pass

class TemplateRender:
    __loader: jinja2.BaseLoader
    __environment: jinja2.Environment

    def __init__(self) -> None:
        self.__loader = jinja2.FileSystemLoader(utils.get_template_directory())
        self.__environment = jinja2.Environment(loader=self.__loader)

    def __render(self, template_name: str, dest_filename: str, payload: BEnumCollection, extra: dict[str, typing.Any] = {}) -> None:
        # prepare template argument
        template_argument: dict[str, typing.Any] = {
            'payload': payload,
            'extra': extra,
            'utils': RenderUtils,
        }
        # fetch template
        template = self.__environment.get_template(str(template_name))
        # render template and save
        with open(utils.get_output_file_path(dest_filename), 'w', encoding='utf-8') as f:
            f.write(template.render(**template_argument))

    def __wrap_single_enum(self, benum: BEnum) -> BEnumCollection:
        return BEnumCollection([benum, ])

    # region: C++ Header and Sources

    def render_cpp_enums(self, filename: str, benums: BEnumCollection) -> None:
        self.__render('generic.hpp.jinja', filename, benums)

    def render_cpp_enum(self, filename: str, benum: BEnum) -> None:
        self.render_cpp_enums(filename, self.__wrap_single_enum(benum))

    def render_cpp_enum_docstrings(self, hpp_filename: str, cpp_filename: str, benums: BEnumCollection, parts: CKParts) -> None:
        self.__render('generic.docstring.hpp.jinja', hpp_filename, benums, {'parts': parts})
        self.__render('generic.docstring.cpp.jinja', cpp_filename, benums, {'parts': parts})
        
    def render_cpp_enum_docstring(self, hpp_filename: str, cpp_filename: str, benum: BEnum, parts: CKParts) -> None:
        self.render_cpp_enum_docstrings(hpp_filename, cpp_filename, self.__wrap_single_enum(benum), parts)

    def render_cpp_ckerror_docstring(self, hpp_filename: str, cpp_filename: str, benums: BEnum) -> None:
        self.__render('CKERROR.docstring.hpp.jinja', hpp_filename, self.__wrap_single_enum(benums))
        self.__render('CKERROR.docstring.cpp.jinja', cpp_filename, self.__wrap_single_enum(benums))
    
    def render_cpp_ckclassid_docstring(self, hpp_filename: str, cpp_filename: str, benums: BEnum) -> None:
        self.__render('CK_CLASSID.docstring.hpp.jinja', hpp_filename, self.__wrap_single_enum(benums))
        self.__render('CK_CLASSID.docstring.cpp.jinja', cpp_filename, self.__wrap_single_enum(benums))

    # endregion

    # region: Python

    def render_py_enums(self, filename: str, benums: BEnumCollection) -> None:
        self.__render('generic.py.jinja', filename, benums)

    def render_py_enum(self, filename: str, benum: BEnum) -> None:
        self.render_py_enums(filename, self.__wrap_single_enum(benum))

    def render_py_enum_docstrings(self, filename: str, benums: BEnumCollection) -> None:
        self.__render('generic.docstring.py.jinja', filename, benums)

    def render_py_enum_docstring(self, filename: str, benum: BEnum) -> None:
        self.render_py_enum_docstrings(filename, self.__wrap_single_enum(benum))

    # endregion

    # region: C#

    def render_cs_enums(self, filename: str, benums: BEnumCollection) -> None:
        self.__render('generic.cs.jinja', filename, benums)

    def render_cs_enum(self, filename: str, benum: BEnum) -> None:
        self.render_cs_enums(filename, self.__wrap_single_enum(benum))

    def render_cs_enum_docstrings(self, filename: str, benums: BEnumCollection) -> None:
        self.__render('generic.docstring.cs.jinja', filename, benums)

    def render_cs_enum_docstring(self, filename: str, benum: BEnum) -> None:
        self.render_cs_enum_docstrings(filename, self.__wrap_single_enum(benum))

    # endregion

    # region: Rust

    def render_rs_enums(self, filename: str, benums: BEnumCollection) -> None:
        self.__render('generic.rs.jinja', filename, benums)

    def render_rs_enum(self, filename: str, benum: BEnum) -> None:
        self.render_rs_enums(filename, self.__wrap_single_enum(benum))

    def render_rs_enum_docstrings(self, filename: str, benums: BEnumCollection) -> None:
        self.__render('generic.docstring.rs.jinja', filename, benums)

    def render_rs_enum_docstring(self, filename: str, benum: BEnum) -> None:
        self.render_rs_enum_docstrings(filename, self.__wrap_single_enum(benum))

    # endregion