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

#ifndef LPMODEL_H
#define LPMODEL_H

#include <vector>
#include <fstream>
#include <iostream>
#include <functional>

#include <set>

#include "Bound.h"
#include "Constraint.h"

namespace lpcompare {

	/**
	\class LPModel
	Represents an LP model composed of a bounds, constraints and variables of different kinds.
	*/

	const char SEPS [] = { ' ' };  /**< Delimiter for bounds. */
	const char SEPS_CONS [] = { ' ', '\t', '\n', '\r' };  /**< Delimiter for constraints. */

	class LPModel {

		template<typename T, typename F>
		std::string* ReadVars(
			std::istream &file,
			std::vector<T>& list,
			F operation);

		std::string* ReadGenerals(std::istream &file);
		std::string* ReadBinaries(std::istream &file);
		std::string* ReadSosVars(std::istream &file);
		std::string* ReadBounds(std::istream &file);
		std::string* ReadConstraints(std::istream &file);

		long linesRead = 0; /**< Counter for lines read. */

		/**
		Increases linesRead counter by one. Prints linesRead to std::cout every million lines.
		*/
		void IncLineCount()
		{
			linesRead++;

			if (linesRead % 1000000 == 0) {
				std::cout << "Lines read: " << linesRead << std::endl;
			}
		}

	public:

		std::vector<std::string> Generals;
		std::vector<std::string> Binaries;
		std::vector<std::string> SosVars;
		std::vector<Bound> Bounds;
		std::vector<Constraint> Constraints;

		LPModel() {
			Generals = std::vector<std::string>();
			Binaries = std::vector<std::string>();
			SosVars = std::vector<std::string>();
			Bounds = std::vector<Bound>();
			Constraints = std::vector<Constraint>();
		}
		~LPModel() {}

		bool ReadModel(std::string filename);
	};
}

#endif // LPMODEL_H