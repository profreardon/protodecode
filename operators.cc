#include "operators.h"

namespace protodecode {

Operator* Operator::create_operator(list<string>* tokens) {
	assert(tokens && tokens->size());
	string name = tokens->front();
	tokens->pop_front();

	if (name == "expect") return new ExpectOperator(tokens);
        if (name == "align") return new AlignOperator(tokens);
        if (name == "padding") return new PaddingOperator(tokens);
	if (name == "string") return new StringOperator(tokens);
	if (name == "lrstring") return new LrstringOperator(tokens);
	if (name == "lpstring") return new LpstringOperator(tokens);
	if (name == "mul") return new MathOperator("mul", tokens);
	if (name == "add") return new MathOperator("add", tokens);
	if (name == "sub") return new MathOperator("sub", tokens);
	if (name == "lsh") return new MathOperator("lsh", tokens);
	if (name == "rsh") return new MathOperator("rsh", tokens);
	if (name == "u1") return new U1Operator(tokens);
	if (name == "bool") return new U1Operator(tokens);
	if (name == "u2") return new U2Operator(tokens);
	if (name == "u4") return new U4Operator(tokens);
	if (name == "u8") return new U8Operator(tokens);
	if (name == "u16") return new U16Operator(tokens);
	if (name == "u32") return new U32Operator(tokens);
	if (name == "u64") return new U64Operator(tokens);
//              if (name == "protocol") return new ProtocolOperator(tokens);
	throw logic_error("unknown name: " + name);
}

}  // namespace protodecode
