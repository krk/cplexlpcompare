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

#include "Constraint.h"
#include "Split.h"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <bitset>
#include <string>

/**
\file Constraint.cpp
Defines Constraint class.
*/

using namespace boost;
using namespace boost::algorithm;

namespace lpcompare {

	/**
	Finds a ConstraintOp representation for the string op.

	\param op Operation to find ConstraintOp for.
	\return A ConstraintOp representing the operation.
	*/
	ConstraintOp get_constraintop(std::string op) {
		if (op == "=")
			return ConstraintOp::EQ;
		if (op == ">=")
			return ConstraintOp::GTE;
		if (op == ">")
			return ConstraintOp::GT;
		if (op == "<=")
			return ConstraintOp::LTE;
		if (op == "<")
			return ConstraintOp::LT;

		return ConstraintOp::EQ;
	}

	/**
	Finds a string representation for the ConstraintOp.

	\param op Operation to find string for.
	\return A string representing the ConstraintOp.
	*/
	std::string get_constraintop(ConstraintOp op) {
		if (op == ConstraintOp::EQ)
			return "=";
		if (op == ConstraintOp::GTE)
			return ">=";
		if (op == ConstraintOp::GT)
			return ">";
		if (op == ConstraintOp::LTE)
			return "<=";
		if (op == ConstraintOp::LT)
			return "<";

		return "=";
	}

	/**
	Splits a string into a vector.

	\param s String to split.
	\param d Delimiter array.
	\param ret Splitted string.
	*/
	void custom_split(std::string & s, char const* d, std::vector<std::string>& ret)
	{
		std::vector<std::string> output;

		std::bitset<255> delims;
		while (*d)
		{
			unsigned char code = *d++;
			delims[code] = true;
		}
		typedef std::string::const_iterator iter;
		iter beg;
		bool in_token = false;
		for (std::string::const_iterator it = s.begin(), end = s.end();
			it != end; ++it)
		{
			if (delims[*it])
			{
				if (in_token)
				{
					output.push_back(std::vector<std::string>::value_type(beg, it));
					in_token = false;
				}
			}
			else if (!in_token)
			{
				beg = it;
				in_token = true;
			}
		}
		if (in_token){
            iter end = s.end();
			output.push_back(std::vector<std::string>::value_type(beg, end));
        }

		output.swap(ret);
	}

	/**
	Parses a line of the LP file representing a Constraint.

	\param line Single line of an LP file.
	\return A Constraint instance representing line.
	*/
	Constraint *Constraint::Parse(std::string& line){

		Constraint *ret = new Constraint();

		std::vector<std::string> parts;

		custom_split(line, " \t\r\n", parts);

		if (parts.size() == 0)
			return nullptr;

		if (contains(parts[0], ":"))
			ret->Name = parts[0].substr(0, parts[0].length() - 1);

		char opChar = '+';
		double coeff = 1;
		std::string name = "";

		for (auto it = parts.begin() + 1; it != parts.end() - 2; ++it)
		{
			auto token = *it;

			if (name.length() > 0)
			{
				// add CoeffVar.
				auto term = new Term();
				term->varName = name;
				term->coeff = opChar == '+' ? coeff : -coeff;
				ret->Terms.push_back(*term);

				name = "";
				opChar = '+';
				coeff = 1;
			}

			if (token.length() == 1 && (token[0] == '-' || token[0] == '+'))
			{
				opChar = token[0];
			}

			if (isdigit(token[0]))
			{
				coeff = boost::lexical_cast<double>(token);
			}

			if (isalpha(token[0]))
			{
				name = token;
			}
		}

		if (name.length() > 0)
		{
			// add CoeffVar.
			auto term = new Term();
			term->varName = name;
			term->coeff = opChar == '+' ? coeff : -coeff;
			ret->Terms.push_back(*term);

			name = "";
			opChar = '+';
			coeff = 1;
		}

		std::sort(ret->Terms.begin(), ret->Terms.end());

		auto opstr = parts[parts.size() - 2];
		auto rhsstr = parts[parts.size() - 1];

		ret->Sign = get_constraintop(opstr);
		ret->RHS = boost::lexical_cast<double>(rhsstr);

		return ret;
	}

	/**
	Compares two Constraint instances for equality.

	\param other Other instance to compare self to.
	\return true if Constraint instances are equivalent.
	*/
	bool Constraint::operator==(const Constraint &other) const {
		bool eq = other.Sign == Sign;
		eq = eq && other.RHS == RHS;
		eq = eq && other.Terms.size() == Terms.size();
		eq = eq && std::equal(other.Terms.begin(), other.Terms.end(), Terms.begin());
		return eq;
	}

	/**
	Compares two Constraint instances for equality.

	\param other Other instance to compare self to.
	\return true if Constraint instances are not equivalent.
	*/
	bool Constraint::operator!=(const Constraint &other) const {
		return !(*this == other);
	}

	/**
	Returns an integer rank representing a ConstraintOp. Equivalent 
	ConstraintOp instances have the same rank.

	\param op ConstraintOp to find rank of.
	\return An integer rank.
	*/
	int get_constraintop_val(ConstraintOp op) {
		if (op == ConstraintOp::EQ)
			return 0;
		if (op == ConstraintOp::GT || op == ConstraintOp::GTE)
			return 1;
		if (op == ConstraintOp::LT || op == ConstraintOp::LTE)
			return 2;

		return 0;
	}

	/**
	Compares two Constraint instances.

	\param other Other instance to compare self to.
	\return true if self is less than other.
	*/
	bool Constraint::operator<(const Constraint &other) const {

		auto sign = get_constraintop_val(Sign);
		auto sign_other = get_constraintop_val(other.Sign);

		if (sign > sign_other)
			return false;

		if (sign_other == sign && RHS > other.RHS)
			return false;

		if (sign_other == sign && other.RHS == RHS && Terms.size() > other.Terms.size())
			return false;

		/* Terms assumed sorted. */
		if (sign_other == sign && RHS == other.RHS && Terms.size() == other.Terms.size()) {
			for (auto it = Terms.begin(), it2 = other.Terms.begin(); it != Terms.end() && it2 != other.Terms.end(); ++it, ++it2) {
				if (*it2 < *it)
					return false;
			}
		}
		if (other == *this)
			return false;

		return true;
	}

	/**
	Dumps a Constraint instance to an ostream in a text format.
	\param cons Constraint to dump.
	\param out
	*/
	void Constraint::dump(const Constraint &cons, std::ostream &out) {

		out << " Name: " << cons.Name << std::endl;
		out << "  " << cons.RHS << " " << get_constraintop(cons.Sign) << std::endl;

		for (auto term : cons.Terms) {
			out << "  " << term.coeff << " * " << term.varName << std::endl;
		}

		out << std::endl;
	}
}
