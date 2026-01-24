import java.util.Collections;
import java.util.Vector;
import java.util.stream.Collectors;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class ExpFctsWalker extends ExpFctsParserBaseListener {

	public ExpFctsWalker() {
		mFctList = new Vector<ExpFctDecl>();
		mCurrentFct = null;
		mCurrentParam = null;
	}

	public Vector<ExpFctDecl> getResult() {
		return mFctList;
	}

	private Vector<ExpFctDecl> mFctList;
	private ExpFctDecl mCurrentFct;
	private ExpFctParamDecl mCurrentParam;

	@Override
	public void enterProgram(ExpFctsParser.ProgramContext ctx) {
		mFctList.clear();
	}

	@Override
	public void enterFctDecl(ExpFctsParser.FctDeclContext ctx) {
		mCurrentFct = new ExpFctDecl();
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
		mFctList.add(mCurrentFct);
		mCurrentFct = null;
	}

	@Override
	public void exitFctArgFileDecl(ExpFctsParser.FctArgFileDeclContext ctx) {
		ExpFctParamDecl decl = new ExpFctParamDecl();
		decl.mVarName = ctx.EXPFCTS_IDENTIFIER().getText();
		decl.mVarDesc = "The pointer to corresponding BMFile.";
		decl.mIsInput = true;
		decl.mVarType.fromCType("BMap::BMFile*");
		mCurrentFct.mFctParams.add(decl);
	}

	@Override
	public void exitFctArgMeshTransDecl(ExpFctsParser.FctArgMeshTransDeclContext ctx) {
		ExpFctParamDecl decl = new ExpFctParamDecl();
		decl.mVarName = ctx.EXPFCTS_IDENTIFIER().getText();
		decl.mVarDesc = "The pointer to corresponding BMMeshTransition.";
		decl.mIsInput = true;
		decl.mVarType.fromCType("BMap::BMMeshTransition*");
		mCurrentFct.mFctParams.add(decl);
	}

	@Override
	public void exitFctArgObjDecl(ExpFctsParser.FctArgObjDeclContext ctx) {
		ExpFctParamDecl first_decl = new ExpFctParamDecl();
		first_decl.mVarName = ctx.EXPFCTS_IDENTIFIER(0).getText();
		first_decl.mVarDesc = "The pointer to corresponding BMFile.";
		first_decl.mIsInput = true;
		first_decl.mVarType.fromCType("BMap::BMFile*");
		mCurrentFct.mFctParams.add(first_decl);

		ExpFctParamDecl second_decl = new ExpFctParamDecl();
		second_decl.mVarName = ctx.EXPFCTS_IDENTIFIER(1).getText();
		second_decl.mVarDesc = "The CKID of object you accessing.";
		second_decl.mIsInput = true;
		second_decl.mVarType.fromCType("LibCmo::CK2::CK_ID");
		mCurrentFct.mFctParams.add(second_decl);
	}

	@Override
	public void enterFctArgParamIn(ExpFctsParser.FctArgParamInContext ctx) {
		mCurrentParam = new ExpFctParamDecl();
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
		mCurrentParam = new ExpFctParamDecl();
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
