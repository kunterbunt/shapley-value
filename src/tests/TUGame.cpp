//
// Created by Sebastian Lindner on 8/3/17.
//

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include "shapley.h"
#include "TUGame.h"

using namespace Shapley;

class TUGameShapleyTests : public CppUnit::TestFixture {
  private:
	TUGamePlayer* playerA = nullptr;
	TUGamePlayer* playerB = nullptr;
	TUGamePlayer* playerC = nullptr;
	double totalBandwidth = 200.0, demandA = 100.0, demandB = 200.0, demandC = 300.0;

  public:
	void setUp() override {
		playerA = new TUGamePlayer(demandA, 0, 0);
		playerB = new TUGamePlayer(demandB, 0, 0);
		playerC = new TUGamePlayer(demandC, 0, 0);
	}

	void tearDown() override {
		delete playerA;
		delete playerB;
		delete playerC;
	}

	void testCharacteristicFunction() {
		std::cout << "[TUGameShapleyTests/testCharacteristicFunction]" << std::endl;
		Coalition<TUGamePlayer> allPlayers;
		allPlayers.add(playerA);
		allPlayers.add(playerB);
		allPlayers.add(playerC);
		TUGameCharacteristicFunction charFunc(&allPlayers, totalBandwidth);
		Coalition<TUGamePlayer> coalition;
		coalition.add(playerB);
		coalition.add(playerC);
		CPPUNIT_ASSERT_EQUAL(100.0, charFunc.getValue(coalition));
		coalition.add(playerA);
		CPPUNIT_ASSERT_EQUAL(totalBandwidth, charFunc.getValue(coalition));
		coalition.remove(playerC);
		CPPUNIT_ASSERT_EQUAL(0.0, charFunc.getValue(coalition));
		coalition.remove(playerB);
		CPPUNIT_ASSERT_EQUAL(0.0, charFunc.getValue(coalition));
		coalition.remove(playerA);
		CPPUNIT_ASSERT_EQUAL(0.0, charFunc.getValue(coalition));
	}

	void testShapleyValues() {
		std::cout << "[TUGameShapleyTests/testShapleyValues]" << std::endl;
		Coalition<TUGamePlayer> allPlayers;
		allPlayers.add(playerA);
		allPlayers.add(playerB);
		allPlayers.add(playerC);
		TUGameCharacteristicFunction charFunc(&allPlayers, totalBandwidth);
		std::vector<const TUGamePlayer*> players;
		players.push_back(playerA);
		players.push_back(playerB);
		players.push_back(playerC);
		std::map<const TUGamePlayer*, double> shapleyValues = compute(players, charFunc);
		CPPUNIT_ASSERT_EQUAL(33.0 + (1.0/3.0), shapleyValues[playerA]);
		CPPUNIT_ASSERT_EQUAL(83.0 + (1.0/3.0), shapleyValues[playerB]);
		CPPUNIT_ASSERT_EQUAL(83.0 + (1.0/3.0), shapleyValues[playerC]);
	}

	CPPUNIT_TEST_SUITE(TUGameShapleyTests);
		CPPUNIT_TEST(testCharacteristicFunction);
		CPPUNIT_TEST(testShapleyValues);
	CPPUNIT_TEST_SUITE_END();
};
