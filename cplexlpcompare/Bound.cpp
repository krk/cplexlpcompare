// Copyright (c) 2014 Kerem KAT 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files(the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Do not hesisate to contact me about usage of the code or to make comments 
// about the code. Your feedback will be appreciated.
//
// http://dissipatedheat.com/
// http://github.com/krk/

#include "Bound.h"
#include "Split.h"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost\algorithm\string.hpp>

using namespace boost;

namespace lpcompare {

	regex re_varname("[a-zA-Z!\"#$%&\\(\\)/,;?@_`'{}|~][a-zA-Z0-9!\"#$%&\\(\\)/,;?@_`'{}|~.]*");

	BoundOp invert(BoundOp op) {
		switch (op){
		case BoundOp::GT:
		case BoundOp::GTE:
			return BoundOp::LTE;

		case BoundOp::LT:
		case BoundOp::LTE:
			return BoundOp::GTE;
		}

		return BoundOp::Free;
	}

	BoundOp get_boundop(std::string op) {
		if (op == "=")
			return BoundOp::EQ;
		if (op == ">=")
			return BoundOp::GTE;
		if (op == ">")
			return BoundOp::GT;
		if (op == "<=")
			return BoundOp::LTE;
		if (op == "<")
			return BoundOp::LT;
		if (op == "Free")
			return BoundOp::Free;

		return BoundOp::Free;
	}

	std::string get_boundop(BoundOp op) {
		if (op == BoundOp::EQ)
			return "=";
		if (op == BoundOp::GTE)
			return ">=";
		if (op == BoundOp::GT)
			return ">";
		if (op == BoundOp::LTE)
			return "<=";
		if (op == BoundOp::LT)
			return "<";
		if (op == BoundOp::Free)
			return "Free";

		return "Free";
	}

	bool Bound::operator==(const Bound &other) const {
		return other.VarName == VarName
			&& other.LB == LB
			&& other.LB_Op == LB_Op
			&& other.UB == UB
			&& other.UB_Op == UB_Op;
	}

	bool Bound::operator<(const Bound &other) const {
		if (VarName > other.VarName)
			return false;

		if (VarName == other.VarName && LB > other.LB)
			return false;

		if (VarName == other.VarName && LB == other.LB && LB_Op > other.LB_Op)
			return false;

		if (VarName == other.VarName && LB == other.LB && LB_Op == other.LB_Op && UB > other.UB)
			return false;

		if (VarName == other.VarName && LB == other.LB && LB_Op == other.LB_Op && UB == other.UB && UB_Op > other.UB_Op)
			return false;

		if (other == *this)
			return false;

		return true;
	}

	bool Bound::operator!=(const Bound &other) const {
		return !(*this == other);
	}

	Bound *Bound::Parse(std::string line){

		Bound *ret = new Bound();

		std::vector<std::string> parts;
		trim(line);
		::split(line, ' ', parts, [](const std::string& s) -> bool { return s.length() > 0; });

		if (parts.size() == 0)
			return nullptr;

		// clean this setBounds mess.
		auto setBounds = [&ret](BoundOp op, std::string s, bool inverted) {
			auto check_op = inverted ? invert(op) : op;

			if (op == BoundOp::EQ) {
				ret->LB = boost::lexical_cast<float>(s);
				ret->UB = ret->LB;
				ret->LB_Op = op;
				ret->UB_Op = op;
			}
			else if (op == BoundOp::GT || op == BoundOp::GTE || (check_op == BoundOp::GT || check_op == BoundOp::GTE)) {
				ret->UB = boost::lexical_cast<float>(s);
				ret->UB_Op = op;
			}
			else if (op == BoundOp::LT || op == BoundOp::LTE || (check_op == BoundOp::LT || check_op == BoundOp::LTE)) {
				ret->LB = boost::lexical_cast<float>(s);
				ret->LB_Op = op;
			}
		};

		cmatch what;
		bool isVarName = regex_match(parts[0].c_str(), what, re_varname);

		if (isVarName){
			ret->VarName = parts[0];

			auto op = get_boundop(parts[1]);
			if (op != BoundOp::Free)
				setBounds(invert(op), parts[2], false);
		}
		else{
			auto op = get_boundop(parts[1]);
			ret->VarName = parts[2];

			if (op != BoundOp::Free)
				setBounds(op, parts[0], false);

			// does have a second part?
			if (parts.size() == 5)
			{
				op = get_boundop(parts[3]);

				if (op != BoundOp::Free)
					setBounds(invert(op), parts[4], true);
			}
		}

		return ret;
	}

	void Bound::dump(const Bound &bound, std::ostream &out) {

		out << bound.LB << " " << get_boundop(bound.LB_Op) << " ";
		out << bound.VarName;

		if (bound.UB != INFTY)
			out << " " << get_boundop(invert(bound.UB_Op)) << " " << bound.UB;
	}
}