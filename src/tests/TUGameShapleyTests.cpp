//
// Created by Sebastian Lindner on 8/3/17.
//

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include "shapley.hpp"

using namespace Shapley;

class TUGameShapleyTests : public CppUnit::TestFixture {
  private:

	class TUGamePlayer : public Player {
	  public:
		TUGamePlayer(double bandwidthDemand, unsigned int connectionId, unsigned short nodeId)
				: bandwidthDemand(bandwidthDemand), connectionId(connectionId), nodeId(nodeId) {}

		double getContribution() const override {
			return bandwidthDemand;
		}

		double getBandwidthDemand() const {
			return bandwidthDemand;
		}

		unsigned int getConnectionId() const {
			return connectionId;
		}

		unsigned short getNodeId() const {
			return nodeId;
		}

	  protected:
		double bandwidthDemand = 0.0;
		unsigned int connectionId = 0;
		unsigned short nodeId = 0;
	};

	class TUGameCharacteristicFunction : public CharacteristicFunction<TUGamePlayer> {
	  public:
		/**
		 * @param allPlayers The characteristic function for a given coalition needs to sum up the demands of all players *not* in that coalition, hence we need a reference to all players.
		 */
		TUGameCharacteristicFunction(Coalition<TUGamePlayer>* allPlayers, double bandwidth)
				: allPlayers(allPlayers), availableBandwidth(bandwidth) {}

		/**
		 * @param coalition
		 * @return A coalition’s worth is what is left, if anything, once the players outside the coalition have received their own claims.
		 */
		double getValue(const Coalition<TUGamePlayer>& coalition) const override {
			double aggregateDemand = 0.0;
			for (const TUGamePlayer* player : allPlayers->getMembers()) {
				if (!coalition.contains(player))
					aggregateDemand += player->getBandwidthDemand();
			}
			return std::max(0.0, availableBandwidth - aggregateDemand);
		}

	  protected:
		const Coalition<TUGamePlayer>* allPlayers = nullptr;
		double availableBandwidth = 0.0;
	};

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
