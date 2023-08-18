import org.antlr.v4.runtime.*;

public class CKEnumCommentsHelper {

	enum CommentsPosition {
		Unknown, Precomment, Postcomment
	}

	public CKEnumCommentsHelper(BufferedTokenStream tokenStream) {
		mTokenStream = tokenStream;
		mCommentsPos = CommentsPosition.Unknown;
	}

	private BufferedTokenStream mTokenStream;
	private CommentsPosition mCommentsPos;

	public String getComment(Token preToken, Token postToken) {
		switch (mCommentsPos) {
		case Unknown: {
			// if we don't know where is our token,
			// we should assume it is from precomment to postcomment
			// and check it.
			Token precomment = CKCommonHelper.getPreChannelToken(mTokenStream, preToken, CKGeneralLexer.COMMENTS);
			if (precomment != null) {
				mCommentsPos = CommentsPosition.Precomment;
				return CKCommonHelper.cutComment(precomment);
			}
			
			Token postcomment = CKCommonHelper.getPostChannelToken(mTokenStream, postToken, CKGeneralLexer.COMMENTS);
			if (postcomment != null) {
				mCommentsPos = CommentsPosition.Postcomment;
				return CKCommonHelper.cutComment(postcomment);
			}
			
			// really do not have comment, return empty and keep comment pos
			return null;
		}
		case Precomment: {
			Token comment = CKCommonHelper.getPreChannelToken(mTokenStream, preToken, CKGeneralLexer.COMMENTS);
			return CKCommonHelper.cutComment(comment);
		}
		case Postcomment: {
			Token comment = CKCommonHelper.getPostChannelToken(mTokenStream, postToken, CKGeneralLexer.COMMENTS);
			return CKCommonHelper.cutComment(comment);
		}
		default:
			return null;
		}
	}

}
