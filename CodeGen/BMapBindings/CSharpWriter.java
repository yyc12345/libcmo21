import java.io.OutputStreamWriter;
import java.util.Vector;
import java.util.stream.Collectors;

public class CSharpWriter {

	/**
	 * The class represent the C# type corresponding to extracted variable type.
	 */
	private static class CsInteropType {
		public CsInteropType() {
			mMarshalAs = null;
			mCsType = null;
		}

		/**
		 * The argument of MarshalAsAttribute constructor. In generation, this field
		 * should be used like this: "[MarshalAs(THIS)]" (for parameter) or "[return:
		 * MarshalAs(THIS)]" (for return value).
		 */
		public String mMarshalAs;
		/**
		 * The C# type used in interop function declaration for corresponding parameter.
		 */
		public String mCsType;
	}

	/**
	 * C# specified function which get C# used interop MarshalAs constructor
	 * arguments and C# type used in interop function declaration.
	 * 
	 * @param vt The instance of {@linkplain VariableType} for fetching interop
	 *           type.
	 * @return The corresponding interop type of given variable type.
	 */
	private static CsInteropType getCsInteropType(ExpFctParamDecl paramdecl) {
		// get essential variable type properties first
		VariableType vt = paramdecl.mVarType;
		String vt_base_type = vt.getBaseType();
		int vt_pointer_level = vt.getPointerLevel();

		// create return value
		CsInteropType ret = new CsInteropType();

		// use "switch" to check variable type
		switch (vt_base_type) {
		case "CKSTRING":
			// decide direction cookies
			String direction_cookie = "";
			if (paramdecl.mIsInput) {
				direction_cookie = "In";
			} else {
				direction_cookie = "Out";
			}
			// only allow 0 and 1 pointer level for string.
			switch (vt_pointer_level) {
			case 0:
				ret.mMarshalAs = "UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = \"" + direction_cookie + "\"";
				// ret.mMarshalAs = "UnmanagedType.LPUTF8Str";
				ret.mCsType = "string";
				break;
			case 1:
				ret.mMarshalAs = "UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringArrayMarshaler), MarshalCookie = \"" + direction_cookie + "\"";
				ret.mCsType = "string[]";
				break;
			}
			break;
		case "CKDWORD":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.U4";
				ret.mCsType = "uint";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "CKWORD":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.U2";
				ret.mCsType = "ushort";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "CKINT":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.I4";
				ret.mCsType = "int";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "bool":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.U1";
				ret.mCsType = "bool";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "CKFLOAT":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.R4";
				ret.mCsType = "float";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "CKBYTE":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.U1";
				ret.mCsType = "byte";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "CK_ID":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.U4";
				ret.mCsType = "uint";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "NakedOutputCallback":
			// callback actually is a function pointer
			// so it only allow base type without any pointer level.
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.FunctionPtr";
				ret.mCsType = "OutputCallback";
			}
			break;
		case "BMFile":
			// In any case, BMFile only should be raw pointer
			if (vt_pointer_level != 0) {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "BMMeshTransition":
			// In any case, BMMeshTransition only should be raw pointer
			if (vt_pointer_level != 0) {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "VxVector3":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.Struct";
				ret.mCsType = "VxVector3";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "VxVector2":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.Struct";
				ret.mCsType = "VxVector2";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "VxColor":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.Struct";
				ret.mCsType = "VxColor";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "VxMatrix":
			if (vt_pointer_level == 0) {
				ret.mMarshalAs = "UnmanagedType.Struct";
				ret.mCsType = "VxMatrix";
			} else {
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		case "CK_TEXTURE_SAVEOPTIONS":
		case "VX_PIXELFORMAT":
		case "VXTEXTURE_BLENDMODE":
		case "VXTEXTURE_FILTERMODE":
		case "VXTEXTURE_ADDRESSMODE":
		case "VXBLEND_MODE":
		case "VXFILL_MODE":
		case "VXSHADE_MODE":
		case "VXCMPFUNC":
		case "VXMESH_LITMODE":
			// all enum type use the same strategy
			if (vt_pointer_level == 0) {
				// all enum type should be marshaled as its underlying type
				// but we can use its name in C# directly.
				ret.mMarshalAs = "UnmanagedType.U4";
				ret.mCsType = vt_base_type;
			} else {
				// for pointer type, use IntPtr instead.
				ret.mMarshalAs = "UnmanagedType.SysInt";
				ret.mCsType = "IntPtr";
			}
			break;
		}

		// check whether we successfully get result
		if (ret.mMarshalAs == null || ret.mCsType == null) {
			throw new IllegalArgumentException("Unexpected type: " + vt.toCType());
		}

		// return value
		return ret;
	}

	public static void writeCSharpCode(Vector<ExpFctDecl> data) throws Exception {
		OutputStreamWriter writer = CommonHelper.openWriter("dest/BMExports.cs");
		IndentHelper helper = new IndentHelper(writer);

		// write function decls
		for (ExpFctDecl fctdecl : data) {
			// write annotation
			// summary (just plain function name)
			helper.printf("/// <summary>%s</summary>", fctdecl.mFctName);
			// parameter list
			for (ExpFctParamDecl paramdecl : fctdecl.mFctParams) {
				helper.printf("/// <param name=\"%s\">Type: %s. %s%s</param>", paramdecl.mVarName,
						paramdecl.mVarType.toCType(), (paramdecl.mIsInput ? "" : "This is OUT parameter. "),
						paramdecl.mVarDesc);
			}
			// return value
			helper.puts("/// <returns>True if no error, otherwise False.</returns>");

			// write real function declaration
			// first, write DllImportAttribute
			helper.printf(
					"[DllImport(g_DllName, EntryPoint = \"%s\", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]",
					fctdecl.mFctName);
			// second, write return value MarshalAsAttribute
			helper.printf("[return: MarshalAs(UnmanagedType.U1)]");
			// then, before we write function body, we need origanize its parameter list
			// first
			Vector<String> cs_param_list = new Vector<String>();
			for (ExpFctParamDecl paramdecl : fctdecl.mFctParams) {
				// create string builder
				StringBuilder sb = new StringBuilder();
				// push in out symbol
				if (paramdecl.mIsInput) {
					sb.append("[In, ");
				} else {
					sb.append("[Out, ");
				}
				// get interop type now
				CsInteropType interop_type = getCsInteropType(paramdecl);
				// push MarshalAsAttribute
				sb.append("MarshalAs(");
				sb.append(interop_type.mMarshalAs);
				sb.append(")] ");
				// push out keyword if parameter is out parameter
				if (!paramdecl.mIsInput) {
					sb.append("out ");
				}
				// push parameter cs type
				sb.append(interop_type.mCsType);
				sb.append(" ");
				// push parameter name
				sb.append(paramdecl.mVarName);
				// insert built string into list
				cs_param_list.add(sb.toString());
			}
			// join built parameter list and output real function declaration
			helper.printf("internal static extern bool %s(%s);", fctdecl.mFctName,
					cs_param_list.stream().collect(Collectors.joining(", ")));
		}

		writer.close();
	}

}
