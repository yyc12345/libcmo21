import jinja2
import jinja2.filters
import re
import typing
from json_loader import BEnumCollection, BEnum
from utils import CKParts
import utils

class RenderUtils:
    """Possible used functions for jinja when rendering templates"""

    TRANTABLE_ESCAPE_STRING: typing.ClassVar[dict[int, str]] = str.maketrans({
        "\\": "\\\\",
        "\t": "\\t",
        "\b": "\\b",
        "\n": "\\n",
        "\r": "\\r",
        "\f": "\\f",
        "\"": "\\\"",
    })

    @staticmethod
    def escape_string(strl: str) -> str:
        """
        Escape string

        Escape all characters which are invalid in string quote.

        :param strl: The string need to be escaped.
        :return: The escaped string.
        """
        return strl.translate(RenderUtils.TRANTABLE_ESCAPE_STRING)

    TRANTABLE_REMOVE_EOL: typing.ClassVar[dict[int, str]] = str.maketrans({
        "\n": "",
        "\r": "",
    })

    @staticmethod
    def remove_eol(strl: str) -> str:
        """
        Remove EOL of given string.

        When rendering code, adding line comment is a common case.
        However, comment may have EOL.
        So when adding this to line comment, we need to remove all EOL.
        """
        return strl.translate(RenderUtils.TRANTABLE_REMOVE_EOL)

    REGEX_PY_TO_LITERAL_NUMBER: typing.ClassVar[re.Pattern] = re.compile("[ulUL]+$")

    @staticmethod
    def convert_to_python_number(numstr: str) -> str:
        """
        Convert accepted string into Python cupported format.

        It actually just remove trail "UL".

        This function is only served for Python code generation.

        :param numstr: The captured number.
        :return: The Python style number string.
        """
        return RenderUtils.REGEX_PY_TO_LITERAL_NUMBER.sub("", numstr, 1)

    REGEX_PY_EXT_HUMANRDABLE_ENTRY_NAME: typing.ClassVar[re.Pattern] = re.compile("^[a-zA-Z0-9]+_")

    @staticmethod
    def extract_human_readable_entry_name(entry_name: str) -> str:
        """
        Try generate human readable name from enum entry name.

        This function is only served for Python code generation.

        As you noticed, almost entries of CK enums are fully capital and splitted by
        underline. This is really not good for human reading, especially those who
        are not programmer. So this function will try give these programmer-oriented
        entry name a human readable name as its display name. However, this extract
        method is not perfect. It simply do some split and replacement so the
        generated content may still not good for reader.

        :param entry_name: The name of enum entry
        :return: A human readable entry name. No guaranteen that return value is must human readable.
        """
        # remove first part (any content before underline '_')
        entry_name = RenderUtils.REGEX_PY_EXT_HUMANRDABLE_ENTRY_NAME.sub("", entry_name, 1)

        # lower all chars except first char
        if len(entry_name) < 1:
            return entry_name
        else:
            return entry_name[0:1] + entry_name[1:].lower()

class TemplateRender:
    """Render templates to code files"""

    __loader: jinja2.BaseLoader
    __environment: jinja2.Environment

    def __init__(self) -> None:
        self.__loader = jinja2.FileSystemLoader(utils.get_template_directory())
        self.__environment = jinja2.Environment(loader=self.__loader)

        # prepare filters
        self.__environment.filters['some_or_blank'] = lambda s: "" if s is None else s
        self.__environment.filters['escape_string'] = lambda s: RenderUtils.escape_string(s)
        self.__environment.filters['block_comment'] = lambda s, fmt: jinja2.filters.do_indent(s, fmt, True, True)
        self.__environment.filters['line_comment'] = lambda s: RenderUtils.remove_eol(s)

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