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

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <string>
#include <vector>
#include "Term.h"

namespace lpcompare {

	/**
	\class Constraint
	Represents a constraint composed of terms, a sign and a right-hand side.
	*/

	enum class ConstraintOp;
	class Constraint;

	enum class ConstraintOp
	{
		EQ = 0,
		GT,
		LT,
		GTE,
		LTE,
	};

	class Constraint{
		ConstraintOp Sign; /**< Sign (operator) of the constraint. */
		double RHS;
		std::vector<Term> *Terms;
	public:
		std::string Name;

		Constraint() 
		{
			Terms = new std::vector<Term>();
		}

		Constraint(Constraint&& other)
			: Terms(nullptr)
		{
			Sign = other.Sign;
			RHS = other.RHS;
			Terms = other.Terms;
		}

		Constraint& operator=(Constraint&& other)
		{
			if (this != &other)
			{
				Sign = other.Sign;
				RHS = other.RHS;
				Terms = other.Terms;

				other.Terms = nullptr;
			}

			return *this;
		}

		static Constraint *Parse(std::string& line);
		bool operator==(const Constraint &other) const;
		bool operator!=(const Constraint &other) const;
		bool operator<(const Constraint &other) const;
		static void dump(const Constraint &cons, std::ostream &out);

		friend std::ostream &operator<<(std::ostream &output, const Constraint &cons)
		{
			dump(cons, output);
			return output;
		}
	};
}

#endif // CONSTRAINT_H