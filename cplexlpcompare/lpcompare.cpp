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

#include <vector>
#include <iostream>
#include "LPModel.h"
#include <boost/range/algorithm/set_algorithm.hpp>
#include <chrono>
#include <boost/program_options.hpp>

#include "Constraint.h"
#include "Term.h"


#define TIMING

#ifdef TIMING
#define INIT_TIMER auto start = std::chrono::high_resolution_clock::now()
#define START_TIMER start = std::chrono::high_resolution_clock::now()
#define STOP_TIMER()  ( \
	std::chrono::duration_cast<std::chrono::milliseconds>( \
	std::chrono::high_resolution_clock::now()-start \
	).count())
#define STOP_TIMER_SEC() (STOP_TIMER() / 1000)
#else
#define INIT_TIMER
#define START_TIMER
#define STOP_TIMER() (0)
#define STOP_TIMER_SEC() (0)
#endif

using std::cout;
using std::endl;
using lpcompare::LPModel;

/**
\file lpcompare.cpp
Handles command line arguments and has the main method.
*/

namespace po = boost::program_options;

template <typename T>
void printCounts(const std::string detail_name, std::vector<T> &vec, std::vector<T> &vecother);

void printStats(LPModel *model);

template <typename T>
void dumpdiff_if_requested(const std::string &detail_name, const std::vector<T> &set1Except2, const std::vector<T> &set2Except1);

std::string first_filename;  /**< Filename of the first LP model. */
std::string second_filename; /**< Filename of the second LP model. */

po::variables_map vm; /**< Variable map for boost::program_options. */

/**
Parses program arguments using boost program_options.

\param argc argument count.
\param argv argument list.
*/
void setup_options(int argc, char *argv []) {

	po::positional_options_description p;
	p.add("first", 1);
	p.add("second", 1);

	po::options_description desc("Usage");
	desc.add_options()
		("help", "show usage information")
		("first", po::value<std::string>(&first_filename)->required(), "model 1 cplex lp file")
		("second", po::value<std::string>(&second_filename)->required(), "model 2 cplex lp file")
		("dump-prefix", po::value<std::string>()->default_value("diffdump"), "filename prefix for difference dumps")
		("dump-diffs", po::value<bool>()->default_value(true), "filename prefix for difference dumps")
		;

	try
	{
		po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			exit(1);
		}

		po::notify(vm);
	}
	catch (po::required_option& e)
	{
		std::cerr << "Error: " << e.what() << std::endl << std::endl;

		cout << desc << "\n";
		exit(1);
	}
}

/**
Creates a filename to dump differences into.

\param model Model name.
\param detail Model detail.
\return dump filename.
*/
std::string get_dump_filename(std::string model, std::string detail) {
	return vm["dump-prefix"].as<std::string>() + "-" + model + "-" + detail + ".log";
}

/**
Checks if diff dumps are requested.

\return true if diff dumps are requested.
*/
bool is_diffdumps_requested() {
	return vm["dump-diffs"].as<bool>();
}

int main(int argc, char *argv [])
{
	INIT_TIMER;

	cout << "lpcompare: Compares two LP files created in cplex format and dumps differences to files." << endl;

	setup_options(argc, argv);

	LPModel* model1 = new LPModel();
	LPModel* model2 = new LPModel();

	START_TIMER;
	cout << "Reading first model: " << first_filename << endl;
	if (!model1->ReadModel(first_filename)) {
		exit(1);
	}
	cout << " First model:" << endl;
	printStats(model1);
	auto first_model_read_sec = STOP_TIMER_SEC();
	cout << " Model read in " << first_model_read_sec << " s" << endl;

	START_TIMER;
	cout << "Reading second model: " << second_filename << endl;
	if (!model2->ReadModel(second_filename)) {
		exit(1);
	}
	cout << " Second Model:" << endl;
	printStats(model2);
	auto second_model_read_sec = STOP_TIMER_SEC();
	cout << " Model read in " << second_model_read_sec << " s" << endl;

	cout << endl;

	printCounts("Generals", model1->Generals, model2->Generals);
	printCounts("Binaries", model1->Binaries, model2->Binaries);
	printCounts("SosVars", model1->SosVars, model2->SosVars);

	START_TIMER;
	printCounts("Bounds", model1->Bounds, model2->Bounds);
	auto bounds_check_sec = STOP_TIMER_SEC();
	cout << " Bounds check completed in " << bounds_check_sec << " s" << endl;

	START_TIMER;
	printCounts("Constraints", model1->Constraints, model2->Constraints);
	auto cons_check_sec = STOP_TIMER_SEC();
	cout << " Constraints check completed in " << cons_check_sec << " s" << endl;

	return 0;
}

/**
Prints statistics for a model.

\param model Model.
*/
void printStats(LPModel *model) {
	cout << "Binaries: " << model->Binaries.size() << endl;
	cout << "Bounds: " << model->Bounds.size() << endl;
	cout << "Constraints: " << model->Constraints.size() << endl;
	cout << "Generals: " << model->Generals.size() << endl;
	cout << "SosVars: " << model->SosVars.size() << endl;
}

/**
Dumps diffs to a file if dumping is requested.

\param set1Except2 List of elements that are in the first model but not in the second model.
\param set2Except1 List of elements that are in the second model but not in the first model.
*/
template <typename T>
void dumpdiff_if_requested(const std::string &detail_name, const std::vector<T> &set1Except2, const std::vector<T> &set2Except1) {

	if (!is_diffdumps_requested())
		return;

	if (set1Except2.size() > 0) {

		auto filename1e2 = get_dump_filename("firstEXCEPTsecond", detail_name);
		std::ofstream out_file_1(filename1e2);

		if (!out_file_1) {
			cout << "Cannot open or create file for writing: " << filename1e2;
			return;
		}

		out_file_1 << "firstEXCEPTsecond " << detail_name << std::endl;
		for (auto item : set1Except2) {
			out_file_1 << item << std::endl;
		}

		out_file_1.flush();
		out_file_1.close();

		cout << "firstEXCEPTsecond diff written for " << detail_name << " to " << filename1e2 << std::endl;
	}

	if (set2Except1.size() > 0) {

		auto filename2e1 = get_dump_filename("secondEXCEPTfirst", detail_name);
		std::ofstream out_file_2(filename2e1);

		if (!out_file_2) {
			cout << "Cannot open or create file for writing: " << filename2e1;
			return;
		}

		out_file_2 << "secondEXCEPTfirst " << detail_name << std::endl;
		for (auto item : set2Except1) {
			out_file_2 << item << std::endl;
		}

		out_file_2.flush();
		out_file_2.close();

		cout << "secondEXCEPTfirst diff written for " << detail_name << " to " << filename2e1 << std::endl;
	}
}

/**
Prints count of a given detail for both models, including differences.

\param detail_name Given name of the detail.
\param vec List of elements that are in the first model.
\param vecother List of elements that are in the second model.
*/
template <typename T>
void printCounts(const std::string detail_name, std::vector<T> &vec, std::vector<T> &vecother)
{
	cout << detail_name << " First Model: " << vec.size() << endl;
	cout << detail_name << " Second Model: " << vecother.size() << endl;

	std::sort(vec.begin(), vec.end());
	assert(std::is_sorted(vec.begin(), vec.end()));

	std::sort(vecother.begin(), vecother.end());
	assert(std::is_sorted(vecother.begin(), vecother.end()));

	std::vector<T> set1Except2;
	std::set_difference(vec.begin(), vec.end(), vecother.begin(), vecother.end(), std::back_inserter(set1Except2));

	std::vector<T> set2Except1;
	std::set_difference(vecother.begin(), vecother.end(), vec.begin(), vec.end(), std::back_inserter(set2Except1));

	auto size1e2 = set1Except2.size();
	auto size2e1 = set1Except2.size();

	if (size1e2 == 0 && size2e1 == 0) {
		cout << detail_name << " are equivalent." << endl;
	}
	else {
		cout << detail_name << " First except Second: " << set1Except2.size() << endl;
		cout << detail_name << " Second except First: " << set2Except1.size() << endl;
	}

	dumpdiff_if_requested(detail_name, set1Except2, set2Except1);
}
