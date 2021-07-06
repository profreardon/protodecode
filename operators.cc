#include "operators.h"
#include "protocol_library.h"

namespace protodecode {

Operator* Operator::create_operator(list<string>* tokens) {
	assert(tokens && tokens->size());
	string name = tokens->front();
	tokens->pop_front();
	cout << "create operator " << name << endl;

	if (name == "{") return new SequenceOperator(tokens);
	if (name == "}") return nullptr;
	if (name == "print") return new PrintOperator(tokens);
	if (name == "call") return new CallOperator(tokens);
	if (name == "for") return new ForOperator(tokens);
	if (name == "while") return new WhileOperator(tokens);
	if (name == "eq") return new ConditionOperator(name, tokens);
	if (name == "le") return new ConditionOperator(name, tokens);
	if (name == "ge") return new ConditionOperator(name, tokens);
	if (name == "lt") return new ConditionOperator(name, tokens);
	if (name == "gt") return new ConditionOperator(name, tokens);
	if (name == "ne") return new ConditionOperator(name, tokens);
	if (name == "expect") return new ExpectOperator(tokens);
        if (name == "align") return new AlignOperator(tokens);
        if (name == "padding") return new PaddingOperator(tokens);
	if (name == "string") return new StringOperator(tokens);
	if (name == "lrstring") return new LrstringOperator(tokens);
	if (name == "lpstring") return new LpstringOperator(tokens);
	if (name == "htons") return new UnaryOperator(name, tokens);
	if (name == "htonl") return new UnaryOperator(name, tokens);
	if (name == "mul") return new MathOperator(name, tokens);
	if (name == "add") return new MathOperator(name, tokens);
	if (name == "sub") return new MathOperator(name, tokens);
	if (name == "lsh") return new MathOperator(name, tokens);
	if (name == "rsh") return new MathOperator(name, tokens);
	if (name == "u1") return new U1Operator(tokens);
	if (name == "bool") return new U1Operator(tokens);
	if (name == "u2") return new U2Operator(tokens);
	if (name == "u4") return new U4Operator(tokens);
	if (name == "u8") return new U8Operator(tokens);
	if (name == "u16") return new U16Operator(tokens);
	if (name == "u32") return new U32Operator(tokens);
	if (name == "u64") return new U64Operator(tokens);
	throw logic_error("unknown name: " + name);
}

void CallOperator::process(ProtocolMap* pmap, ProtocolState* state) {
        Operator* op = ProtocolLibrary::_()->get(_protocol);
        assert(op);
	ProtocolMap subpmap;
	op->process(&subpmap, state);
	pmap->merge(_prefix, subpmap);
}

}  // namespace protodecode
