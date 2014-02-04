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

#include "Split.h"

using namespace std;

std::set<std::string> &split(
	const std::string &s,
	char delim,
	std::set<std::string> &elems,
	std::function<bool(const std::string&)> predicate) {

	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (predicate == nullptr || predicate(item))
			elems.insert(item);
	}
	return elems;
}

std::set<std::string> &split(const std::string &s, char delim, std::set<std::string> &elems) {
	return split(s, delim, elems, nullptr);
}

std::vector<std::string> &split(
	const std::string &s,
	char delim,
	std::vector<std::string> &elems,
	std::function<bool(const std::string&)> predicate) {

	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (predicate == nullptr || predicate(item))
			elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> &split(
	 std::string &s,
	std::string delims,
	std::vector<std::string> &elems,
	std::function<bool(const std::string&)> predicate) {

	boost::trim_if(s, boost::is_any_of(delims));
	boost::split(elems, s, boost::is_any_of(delims));

	return elems;
}



std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	return split(s, delim, elems, nullptr);
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}