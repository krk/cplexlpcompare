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

#include "LPModel.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <algorithm>
#include <boost\iostreams\stream.hpp>
#include <boost\iostreams\stream_buffer.hpp>
#include <boost\iostreams\device\mapped_file.hpp>
#include <boost\algorithm\string.hpp>
#include <boost\filesystem.hpp>

#include "Split.h"

using std::ifstream;
using std::cout;
using std::getline;
using namespace boost::algorithm;

namespace lpcompare {

	bool LPModel::ReadModel(std::string filename) {
		
		if (!boost::filesystem::exists(filename)) {
			cout << "File not found: " << filename;
			return false;
		}

		boost::iostreams::stream<boost::iostreams::mapped_file_source> file(filename);

		if (!file){
			cout << "Cannot open file: " << filename;
			return false;
		}

		std::string *line = new std::string();
		bool reconsider;

		getline(file, *line);
		lineCount = 0;
		while (!file.eof()){
			reconsider = false;

			IncLineCount();

			if (line->length() == 0 || (line->length() > 0 && ((*line)[0] == ' ' || (*line)[0] == '\\'))) {
				getline(file, *line);
				continue;
			}

			trim(*line);

			if (boost::iequals(*line, "Generals")
				|| boost::iequals(*line, "General")
				|| boost::iequals(*line, "Gen")) {
				line = ReadGenerals(file);
				reconsider = true;
			}
			else if (boost::iequals(*line, "Bounds")
				|| boost::iequals(*line, "Bound")) {
				line = ReadBounds(file);
				reconsider = true;
			}
			else if (boost::iequals(*line, "Binaries")
				|| boost::iequals(*line, "Binary")
				|| boost::iequals(*line, "Bin")) {
				line = ReadBinaries(file);
				reconsider = true;
			}
			else if (boost::iequals(*line, "SOS")) {
				line = ReadSosVars(file);
				reconsider = true;
			}
			else if (boost::iequals(*line, "Subject To")
				|| boost::iequals(*line, "such that")
				|| boost::iequals(*line, "st")
				|| boost::iequals(*line, "S.T.")
				|| boost::iequals(*line, "ST.")) {
				line = ReadConstraints(file);
				reconsider = true;
			}

			if (!reconsider) {
				getline(file, *line);
			}
		}

		return true;
	}

	template<typename T, typename F>
	std::string* LPModel::ReadVars(
		std::istream &file,
		std::vector<T>& list,
		F operation)
	{
		std::string* line = new std::string();

		getline(file, *line);
		IncLineCount();

		while (!file.eof()) {

			if (line->length() > 0 && ((*line)[0] == '\\')) {
				getline(file, *line);
				IncLineCount();
				continue;
			}

			if (line->length() > 0 && ((*line)[0] != ' ')) {
				return line;
			}

			operation(*line, list);

			getline(file, *line);
			IncLineCount();
		}

		return line;
	}

	std::string* LPModel::ReadGenerals(std::istream &file) {

		return ReadVars(file, Generals, [](const std::string& line, std::vector<std::string>& list) {
			::split(line, ' ', list, [](const std::string& s) { return s.length() > 0 && s[0] != '\r' && s[0] != '\n'; });
		});
	}

	std::string* LPModel::ReadBinaries(std::istream &file) {

		return ReadVars(file, Binaries, [](const std::string& line, std::vector<std::string>& list) {
			::split(line, ' ', list, [](const std::string& s) { return s.length() > 0 && s[0] != '\r' && s[0] != '\n'; });
		});
	}

	std::string* LPModel::ReadSosVars(std::istream &file) {

		return ReadVars(file, SosVars, [](const std::string& line, std::vector<std::string>& list) {
			::split(line, ' ', list, [](const std::string& s) { return s.length() > 0 && s[0] != '\r' && s[0] != '\n'; });
		});
	}

	std::string* LPModel::ReadBounds(std::istream &file) {

		return ReadVars(file, Bounds, [](const std::string& line, std::vector<Bound>& list) {

			if (line.length() > 0 && line[0] != '\r' && line[0] != '\n') {

				auto b = Bound::Parse(line);

				if (b != nullptr) {
					list.push_back(*b);
				}
			}
		});
	}

	std::string* LPModel::ReadConstraints(std::istream &file) {

		std::string* line = new std::string();

		getline(file, *line);
		IncLineCount();

		std::vector<std::string> consraws;
		consraws.reserve(10);

		while (!file.eof()) {

			if (line->length() > 0 && ((*line)[0] == '\\')) {
				getline(file, *line);
				IncLineCount();
				continue;
			}

			if ((*line)[0] != ' ')
				break;

			if ((*line)[1] != ' ' && consraws.size() > 0) {

				std::string rawcons;

				rawcons = "";

				for (auto cons : consraws) {
					rawcons += cons;
					rawcons += " ";
				}

				auto b = Constraint::Parse(rawcons);

				if (b != nullptr) {
					Constraints.push_back(*b);
				}

				consraws.clear();
				//consraws = std::vector<std::string>();
				consraws.reserve(10);
			}

			consraws.push_back(*line);

			getline(file, *line);
			IncLineCount();
		}

		if (consraws.size() > 0) {

			std::string rawcons;

			for (auto cons : consraws) {
				rawcons += cons;
				rawcons += " ";
			}

			auto b = Constraint::Parse(rawcons);

			if (b != nullptr) {
				Constraints.push_back(*b);
			}

			consraws.clear();
		}

		return line;
	}
}