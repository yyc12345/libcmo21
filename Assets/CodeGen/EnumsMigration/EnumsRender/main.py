import json_loader
import template_render
import utils
from utils import CKParts

def main():
    render = template_render.TemplateRender()

    # CKERROR
    ckerror = json_loader.load_enum("CKERROR.json")
    render.render_cpp_enum("CKERROR.hpp", ckerror)
    render.render_py_enum("CKERROR.py", ckerror)
    render.render_cs_enum("CKERROR.cs", ckerror)
    render.render_cpp_ckerror_docstring("CKERROR.docstring.hpp", "CKERROR.docstring.cpp", ckerror)
    render.render_py_enum_docstring("CKERROR.docstring.py", ckerror)
    render.render_cs_enum_docstring("CKERROR.docstring.cs", ckerror)
    render.render_rs_enum_docstring("CKERROR.docstring.rs", ckerror)

    # CK_CLASSID
    classid = json_loader.load_enum("CK_CLASSID.json")
    render.render_cpp_enum("CK_CLASSID.hpp", classid)
    render.render_py_enum("CK_CLASSID.py", classid)
    render.render_cs_enum("CK_CLASSID.cs", classid)
    render.render_rs_enum("CK_CLASSID.rs", classid)
    render.render_cpp_ckclassid_docstring("CK_CLASSID.docstring.hpp", "CK_CLASSID.docstring.cpp", classid)
    render.render_py_enum_docstring("CK_CLASSID.docstring.py", classid)
    render.render_cs_enum_docstring("CK_CLASSID.docstring.cs", classid)
    render.render_rs_enum_docstring("CK_CLASSID.docstring.rs", classid)

    # Define2
    def2 = json_loader.load_enums("Defines2.json")
    render.render_cpp_enums("Defines2.hpp", def2)
    render.render_py_enums("Defines2.py", def2)
    render.render_cs_enums("Defines2.cs", def2)
    render.render_rs_enums("Defines2.rs", def2)
    # Define2 do not need annotation output.
    # Because they are CKStateChunk used value which are not exposed to outside.

    # Combined Enums
    ck2Enums = json_loader.load_enums("CKEnums.json")
    vxEnums = json_loader.load_enums("VxEnums.json")

    render.render_cpp_enums("CKEnums.hpp", ck2Enums)
    render.render_py_enums("CKEnums.py", ck2Enums)
    render.render_cs_enums("CKEnums.cs", ck2Enums)
    render.render_rs_enums("CKEnums.rs", ck2Enums)
    render.render_cpp_enum_docstrings("CKEnums.docstring.hpp", "CKEnums.docstring.cpp", ck2Enums, CKParts.CK2)
    render.render_py_enum_docstrings("CKEnums.docstring.py", ck2Enums)
    render.render_cs_enum_docstrings("CKEnums.docstring.cs", ck2Enums)
    render.render_rs_enum_docstrings("CKEnums.docstring.rs", ck2Enums)

    render.render_cpp_enums("VxEnums.hpp", vxEnums)
    render.render_py_enums("VxEnums.py", vxEnums)
    render.render_cs_enums("VxEnums.cs", vxEnums)
    render.render_rs_enums("VxEnums.rs", vxEnums)
    render.render_cpp_enum_docstrings("VxEnums.docstring.hpp", "VxEnums.docstring.cpp", vxEnums, CKParts.VxMath)
    render.render_py_enum_docstrings("VxEnums.docstring.py", vxEnums)
    render.render_cs_enum_docstrings("VxEnums.docstring.cs", vxEnums)
    render.render_rs_enum_docstrings("VxEnums.docstring.rs", vxEnums)

    # Single enums
    single = json_loader.load_enum("CK_STATECHUNK_CHUNKVERSION.json")
    render.render_cpp_enum("CK_STATECHUNK_CHUNKVERSION.hpp", single)
    render.render_py_enum("CK_STATECHUNK_CHUNKVERSION.py", single)
    render.render_cs_enum("CK_STATECHUNK_CHUNKVERSION.cs", single)
    render.render_rs_enum("CK_STATECHUNK_CHUNKVERSION.rs", single)
    render.render_cpp_enum_docstring("CK_STATECHUNK_CHUNKVERSION.docstring.hpp", "CK_STATECHUNK_CHUNKVERSION.docstring.cpp", single, CKParts.CK2)
    render.render_py_enum_docstring("CK_STATECHUNK_CHUNKVERSION.docstring.py", single)
    render.render_cs_enum_docstring("CK_STATECHUNK_CHUNKVERSION.docstring.cs", single)
    render.render_rs_enum_docstring("CK_STATECHUNK_CHUNKVERSION.docstring.rs", single)

    single = json_loader.load_enum("CK_STATECHUNK_DATAVERSION.json")
    render.render_cpp_enum("CK_STATECHUNK_DATAVERSION.hpp", single)
    render.render_py_enum("CK_STATECHUNK_DATAVERSION.py", single)
    render.render_cs_enum("CK_STATECHUNK_DATAVERSION.cs", single)
    render.render_rs_enum("CK_STATECHUNK_DATAVERSION.rs", single)
    render.render_cpp_enum_docstring("CK_STATECHUNK_DATAVERSION.docstring.hpp", "CK_STATECHUNK_DATAVERSION.docstring.cpp", single, CKParts.CK2)
    render.render_py_enum_docstring("CK_STATECHUNK_DATAVERSION.docstring.py", single)
    render.render_cs_enum_docstring("CK_STATECHUNK_DATAVERSION.docstring.cs", single)
    render.render_rs_enum_docstring("CK_STATECHUNK_DATAVERSION.docstring.rs", single)

    single = json_loader.load_enum("CK_BITMAPDATA_FLAGS.json")
    render.render_cpp_enum("CK_BITMAPDATA_FLAGS.hpp", single)
    render.render_py_enum("CK_BITMAPDATA_FLAGS.py", single)
    render.render_cs_enum("CK_BITMAPDATA_FLAGS.cs", single)
    render.render_rs_enum("CK_BITMAPDATA_FLAGS.rs", single)
    render.render_cpp_enum_docstring("CK_BITMAPDATA_FLAGS.docstring.hpp", "CK_BITMAPDATA_FLAGS.docstring.cpp", single, CKParts.CK2)
    render.render_py_enum_docstring("CK_BITMAPDATA_FLAGS.docstring.py", single)
    render.render_cs_enum_docstring("CK_BITMAPDATA_FLAGS.docstring.cs", single)
    render.render_rs_enum_docstring("CK_BITMAPDATA_FLAGS.docstring.rs", single)

    single = json_loader.load_enum("CK_CAMERA_PROJECTION.json")
    render.render_cpp_enum("CK_CAMERA_PROJECTION.hpp", single)
    render.render_py_enum("CK_CAMERA_PROJECTION.py", single)
    render.render_cs_enum("CK_CAMERA_PROJECTION.cs", single)
    render.render_rs_enum("CK_CAMERA_PROJECTION.rs", single)
    render.render_cpp_enum_docstring("CK_CAMERA_PROJECTION.docstring.hpp", "CK_CAMERA_PROJECTION.docstring.cpp", single, CKParts.CK2)
    render.render_py_enum_docstring("CK_CAMERA_PROJECTION.docstring.py", single)
    render.render_cs_enum_docstring("CK_CAMERA_PROJECTION.docstring.cs", single)
    render.render_rs_enum_docstring("CK_CAMERA_PROJECTION.docstring.rs", single)
    
    print("Done")


if __name__ == "__main__":
    main()
