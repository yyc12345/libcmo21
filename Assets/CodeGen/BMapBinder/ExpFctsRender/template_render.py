import jinja2
import re
import typing
from dataclasses import dataclass
from json_loader import ExpFctCollection, ExpFctParam
import utils

CPP_PY_TYPE_MAP: dict[str, str] = {
    "CKSTRING": "CKSTRING",
    "CKDWORD": "CKDWORD",
    "CKWORD": "CKWORD",
    "CKINT": "CKINT",
    "bool": "bool",
    "CKFLOAT": "CKFLOAT",
    "CKBYTE": "CKBYTE",
    "CK_ID": "CKID",
    "NakedOutputCallback": "callback",
    "BMFile": "void",
    "BMMeshTransition": "void",
    "VxVector3": "VxVector3",
    "VxVector2": "VxVector2",
    "VxColor": "VxColor",
    "VxMatrix": "VxMatrix",
    "CK_TEXTURE_SAVEOPTIONS": "enum",
    "VX_PIXELFORMAT": "enum",
    "VXLIGHT_TYPE": "enum",
    "VXTEXTURE_BLENDMODE": "enum",
    "VXTEXTURE_FILTERMODE": "enum",
    "VXTEXTURE_ADDRESSMODE": "enum",
    "VXBLEND_MODE": "enum",
    "VXFILL_MODE": "enum",
    "VXSHADE_MODE": "enum",
    "VXCMPFUNC": "enum",
    "VXMESH_LITMODE": "enum",
}

CS_ENUM_LIKE: set[str] = set((
    "CK_TEXTURE_SAVEOPTIONS",
    "VX_PIXELFORMAT",
    "VXLIGHT_TYPE",
    "VXTEXTURE_BLENDMODE",
    "VXTEXTURE_FILTERMODE",
    "VXTEXTURE_ADDRESSMODE",
    "VXBLEND_MODE",
    "VXFILL_MODE",
    "VXSHADE_MODE",
    "VXCMPFUNC",
    "VXMESH_LITMODE",
))

@dataclass(frozen=True)
class CsInteropType:
    """The class represent the C# type corresponding to extracted variable type."""

    marshal_as: str
    """
    The argument of MarshalAsAttribute constructor.
    In generation, this field should be used like this: "[MarshalAs(THIS)]" (for parameter),
    or "[return: MarshalAs(THIS)]" (for return value).
    """
    cs_type: str
    """
    The C# type used in interop function declaration for corresponding parameter.
    """


class RenderUtils:
    """Possible used functions for jinja when rendering templates"""

    @staticmethod
    def get_python_type(param: ExpFctParam) -> str:
        vt = param.var_type
        if not param.is_input:
            vt = vt.get_pointer_of_this()

        # add type prefix
        sb: str = "bm_"
        # try getting cpp type from base type and add it
        cpp_type = CPP_PY_TYPE_MAP.get(vt.get_base_type(), None)
        if cpp_type is None:
            raise RuntimeError(f"unexpected type {vt.to_c_type()}")
        else:
            sb += cpp_type
        # add pointer suffix
        if vt.is_pointer():
            sb += "_"
            sb += "p" * vt.get_pointer_level()

        # return built type string.
        return sb

    @staticmethod
    def get_cs_interop_type(param: ExpFctParam) -> CsInteropType:
        # get essential variable type properties first
        vt = param.var_type
        vt_base_type = vt.get_base_type()
        vt_pointer_level = vt.get_pointer_level()

        # declare return value
        marshal_as: str | None = None
        cs_type: str | None = None

        # use "match" to check variable type
        match vt_base_type:
            case 'CKSTRING':
                # decide direction cookies
                direction_cookie = 'In' if param.is_input else 'Out'
                # only allow 0 and 1 pointer level for string.
                match vt_pointer_level:
                    case 0:
                        marshal_as = f'UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "{direction_cookie}"'
                        cs_type = "string"
                    case 1:
                        marshal_as = f'UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringArrayMarshaler), MarshalCookie = "{direction_cookie}"'
                        cs_type = "string[]"
            case "CKDWORD":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.U4"
                    cs_type = "uint"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "CKWORD":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.U2"
                    cs_type = "ushort"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "CKINT":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.I4"
                    cs_type = "int"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "bool":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.U1"
                    cs_type = "bool"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "CKFLOAT":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.R4"
                    cs_type = "float"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "CKBYTE":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.U1"
                    cs_type = "byte"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "CK_ID":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.U4"
                    cs_type = "uint"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "NakedOutputCallback":
                # callback actually is a function pointer
                # so it only allow base type without any pointer level.
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.FunctionPtr"
                    cs_type = "OutputCallback"
            case "BMFile":
                # In any case, BMFile only should be raw pointer
                if vt_pointer_level != 0:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "BMMeshTransition":
                # In any case, BMMeshTransition only should be raw pointer
                if vt_pointer_level != 0:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "VxVector3":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.Struct"
                    cs_type = "VxVector3"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "VxVector2":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.Struct"
                    cs_type = "VxVector2"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "VxColor":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.Struct"
                    cs_type = "VxColor"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case "VxMatrix":
                if vt_pointer_level == 0:
                    marshal_as = "UnmanagedType.Struct"
                    cs_type = "VxMatrix"
                else:
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"
            case enumlike if enumlike in CS_ENUM_LIKE:
                # all enum type use the same strategy
                if vt_pointer_level == 0:
                    # all enum type should be marshaled as its underlying type
                    # but we can use its name in C# directly.
                    marshal_as = "UnmanagedType.U4"
                    cs_type = vt_base_type
                else:
                    # for pointer type, use IntPtr instead.
                    marshal_as = "UnmanagedType.SysInt"
                    cs_type = "IntPtr"

        # check whether we successfully get result
        if marshal_as is None or cs_type is None:
            raise RuntimeError(f'unexpected type: {vt.to_c_type()}')
        
        # return value
        return CsInteropType(marshal_as, cs_type)


class TemplateRender:
    """Render templates to code files"""

    __loader: jinja2.BaseLoader
    __environment: jinja2.Environment

    def __init__(self) -> None:
        self.__loader = jinja2.FileSystemLoader(utils.get_template_directory())
        self.__environment = jinja2.Environment(loader=self.__loader)

    def __render(
        self, template_name: str, dest_filename: str, payload: ExpFctCollection
    ) -> None:
        # prepare template argument
        template_argument: dict[str, typing.Any] = {
            "payload": payload,
            "utils": RenderUtils,
        }
        # fetch template
        template = self.__environment.get_template(str(template_name))
        # render template and save
        with open(utils.get_output_file_path(dest_filename), "w", encoding="utf-8") as f:
            f.write(template.render(**template_argument))

    def render_cs_expfcts(self, filename: str, fcts: ExpFctCollection) -> None:
        self.__render("expfcts.cs.jinja", filename, fcts)

    def render_py_expfcts(self, filename: str, fcts: ExpFctCollection) -> None:
        self.__render("expfcts.py.jinja", filename, fcts)

    def render_rs_expfcts(self, filename: str, fcts: ExpFctCollection) -> None:
        self.__render("expfcts.rs.jinja", filename, fcts)
