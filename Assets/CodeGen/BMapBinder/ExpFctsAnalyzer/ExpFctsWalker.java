import java.util.Collections;
import java.util.stream.Collectors;

public class ExpFctsWalker extends ExpFctsParserBaseListener {

	public ExpFctsWalker() {
		mFctCollection = null;
		mCurrentFct = null;
		mCurrentParam = null;
	}

	public ExpFctsHelper.ExpFctCollection getResult() {
		return mFctCollection;
	}

	private ExpFctsHelper.ExpFctCollection mFctCollection;
	private ExpFctsHelper.ExpFct mCurrentFct;
	private ExpFctsHelper.ExpFctParam mCurrentParam;

	@Override
	public void enterProgram(ExpFctsParser.ProgramContext ctx) {
		mFctCollection = new ExpFctsHelper.ExpFctCollection();
	}

	@Override
	public void enterFctDecl(ExpFctsParser.FctDeclContext ctx) {
		mCurrentFct = new ExpFctsHelper.ExpFct();
	}

	@Override
	public void exitFctDecl(ExpFctsParser.FctDeclContext ctx) {
		// set name
		mCurrentFct.mFctName = ctx.EXPFCTS_IDENTIFIER().getText();
		// check return type
		if (!mCurrentFct.mFctRetType.isValid() || mCurrentFct.mFctRetType.isPointer()
				|| !mCurrentFct.mFctRetType.getBaseType().equals("bool"))
			throw new IllegalArgumentException("invalid interface function return type. must be bool.");
		
		// add into list
		mFctCollection.mFcts.add(mCurrentFct);
		mCurrentFct = null;
	}

	@Override
	public void exitFctArgFileDecl(ExpFctsParser.FctArgFileDeclContext ctx) {
		ExpFctsHelper.ExpFctParam param = new ExpFctsHelper.ExpFctParam();
		param.mVarName = ctx.EXPFCTS_IDENTIFIER().getText();
		param.mVarDesc = "The pointer to corresponding BMFile.";
		param.mIsInput = true;
		param.mVarType.fromCType("BMap::BMFile*");
		mCurrentFct.mFctParams.add(param);
	}

	@Override
	public void exitFctArgMeshTransDecl(ExpFctsParser.FctArgMeshTransDeclContext ctx) {
		ExpFctsHelper.ExpFctParam param = new ExpFctsHelper.ExpFctParam();
		param.mVarName = ctx.EXPFCTS_IDENTIFIER().getText();
		param.mVarDesc = "The pointer to corresponding BMMeshTransition.";
		param.mIsInput = true;
		param.mVarType.fromCType("BMap::BMMeshTransition*");
		mCurrentFct.mFctParams.add(param);
	}

	@Override
	public void exitFctArgObjDecl(ExpFctsParser.FctArgObjDeclContext ctx) {
		ExpFctsHelper.ExpFctParam firstParam = new ExpFctsHelper.ExpFctParam();
		firstParam.mVarName = ctx.EXPFCTS_IDENTIFIER(0).getText();
		firstParam.mVarDesc = "The pointer to corresponding BMFile.";
		firstParam.mIsInput = true;
		firstParam.mVarType.fromCType("BMap::BMFile*");
		mCurrentFct.mFctParams.add(firstParam);

		ExpFctsHelper.ExpFctParam secondParam = new ExpFctsHelper.ExpFctParam();
		secondParam.mVarName = ctx.EXPFCTS_IDENTIFIER(1).getText();
		secondParam.mVarDesc = "The CKID of object you accessing.";
		secondParam.mIsInput = true;
		secondParam.mVarType.fromCType("LibCmo::CK2::CK_ID");
		mCurrentFct.mFctParams.add(secondParam);
	}

	@Override
	public void enterFctArgParamIn(ExpFctsParser.FctArgParamInContext ctx) {
		mCurrentParam = new ExpFctsHelper.ExpFctParam();
	}

	@Override
	public void exitFctArgParamIn(ExpFctsParser.FctArgParamInContext ctx) {
		mCurrentParam.mVarName = ctx.EXPFCTS_IDENTIFIER().getText();
		mCurrentParam.mIsInput = true;

		mCurrentFct.mFctParams.add(mCurrentParam);
		mCurrentParam = null;
	}

	@Override
	public void enterFctArgParamOut(ExpFctsParser.FctArgParamOutContext ctx) {
		mCurrentParam = new ExpFctsHelper.ExpFctParam();
	}

	@Override
	public void exitFctArgParamOut(ExpFctsParser.FctArgParamOutContext ctx) {
		mCurrentParam.mVarName = ctx.EXPFCTS_IDENTIFIER().getText();
		mCurrentParam.mIsInput = false;
		// set to its pointer type
		// mCurrentParam.mVarType = mCurrentParam.mVarType.getPointerOfThis();

		mCurrentFct.mFctParams.add(mCurrentParam);
		mCurrentParam = null;
	}

	@Override
	public void exitVarType(ExpFctsParser.VarTypeContext ctx) {
		// get namespace parts and join them
		String ctype = ctx.EXPFCTS_IDENTIFIER().stream().map(value -> value.getText())
				.collect(Collectors.joining("::"));
		// add star if necessary
		if (ctx.EXPFCTS_STAR() != null) {
			ctype += String.join("", Collections.nCopies(ctx.EXPFCTS_STAR().size(), "*"));
		}

		if (!mCurrentFct.mFctRetType.isValid()) {
			// fill function ret type first
			mCurrentFct.mFctRetType.fromCType(ctype);
		} else {
			// otherwise, fill param data
			mCurrentParam.mVarType.fromCType(ctype);
		}
	}

}
