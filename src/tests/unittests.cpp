//
// Created by Sebastian Lindner on 14.11.17.
//

#include <cppunit/ui/text/TestRunner.h>
#include "tests/ShapleyTests.cpp"
#include "tests/TUGame.cpp"

int main(int argc, const char *argv[]) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(ShapleyTests::suite());
	runner.addTest(TUGameShapleyTests::suite());
	runner.run();
	return 0;
}