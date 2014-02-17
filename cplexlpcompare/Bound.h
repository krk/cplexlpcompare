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

#ifndef BOUND_H
#define BOUND_H

#include <string>

namespace lpcompare {

	/**
	\class Bound
	Represents a bound of a variable.
	*/

#define INFTY (1e+21f) 

	enum class BoundOp;
	class Bound;

	enum class BoundOp
	{
		Free = 0,
		EQ,
		GT,
		LT,
		GTE,
		LTE,
	};

	class Bound{

	public:
		BoundOp UB_Op;
		BoundOp LB_Op;
		float LB;
		float UB;

		std::string VarName;

		Bound() : UB_Op(BoundOp::LTE), LB_Op(BoundOp::LTE), LB(0), UB(INFTY) {}
		static Bound *Parse(std::string line);
		bool operator==(const Bound &other) const;
		bool operator<(const Bound &other) const;
		bool operator!=(const Bound &other) const;

		static void dump(const Bound &cons, std::ostream &out);

		Bound& operator=(const Bound& element) {
			LB = element.LB;
			UB = element.UB;
			UB_Op = element.UB_Op;
			LB_Op = element.LB_Op;
			VarName = element.VarName;

			return *this;
		}

		friend std::ostream &operator<<(std::ostream &output, const Bound &bound)
		{
			dump(bound, output);
			return output;
		}
	};
}

#endif // BOUND_H