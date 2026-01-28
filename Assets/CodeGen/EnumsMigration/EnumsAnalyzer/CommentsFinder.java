import java.util.List;

import org.antlr.v4.runtime.*;

public class CommentsFinder {

	enum CommentsPosition {
		Unknown, Precomment, Postcomment
	}

	public CommentsFinder(BufferedTokenStream tokenStream) {
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
			List<Token> precomment = CommonHelper.getPreChannelTokens(mTokenStream, preToken, CKGenericLexer.COMMENTS);
			if (precomment != null) {
				mCommentsPos = CommentsPosition.Precomment;
				return CommonHelper.cutComments(precomment);
			}
			
			List<Token> postcomment = CommonHelper.getPostChannelTokens(mTokenStream, postToken, CKGenericLexer.COMMENTS);
			if (postcomment != null) {
				mCommentsPos = CommentsPosition.Postcomment;
				return CommonHelper.cutComments(postcomment);
			}
			
			// really do not have comment, return empty and keep comment pos
			return null;
		}
		case Precomment: {
			return CommonHelper.cutComments(CommonHelper.getPreChannelTokens(mTokenStream, preToken, CKGenericLexer.COMMENTS));
		}
		case Postcomment: {
			return CommonHelper.cutComments(CommonHelper.getPostChannelTokens(mTokenStream, postToken, CKGenericLexer.COMMENTS));
		}
		default:
			return null;
		}
	}

}
