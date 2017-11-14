#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include "shapley.h"

using namespace Shapley;

class ShapleyTests : public CppUnit::TestFixture {
  private:
	
	class TaxiPlayer : public Player {
	  public:
		explicit TaxiPlayer(double contribution) : taxiFare(contribution) {}
		
		double getContribution() const override {
			return taxiFare;
		}
	  
	  protected:
		double taxiFare;
	};
	
	class TaxiCharacteristicFunction : public CharacteristicFunction<TaxiPlayer> {
	  public:
		double getValue(const Coalition<TaxiPlayer>& coalition) const override {
			double value = 0.0;
			// Find largest taxi fare of any coalition member.
			for (const TaxiPlayer* member : coalition.getMembers()) {
				if (member->getContribution() > value)
					value = member->getContribution();
			}
			return value;
		}
	};
	
	double fareA = 6.0, fareB = 12.0, fareC = 42.0;
	const TaxiPlayer *playerA = nullptr,
					 *playerB = nullptr,
					 *playerC = nullptr;
	Coalition<TaxiPlayer>* coalition = nullptr;
  
  public:
	void setUp() override {
		playerA = new TaxiPlayer(fareA);
		playerB = new TaxiPlayer(fareB);
		playerC = new TaxiPlayer(fareC);
		coalition = new Coalition<TaxiPlayer>();
		coalition->add(playerA);
		coalition->add(playerB);
		coalition->add(playerC);
	}
	
	void tearDown() override {
		delete coalition;
		delete playerA;
		delete playerB;
		delete playerC;
	}
	
	void testCoalition() {
		std::cout << "[ShapleyTests/testCoalition]" << std::endl;
		CPPUNIT_ASSERT_EQUAL(true, coalition->contains(playerA));
		coalition->remove(playerA);
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerA));
		coalition->add(playerA);
		bool exception = false;
		try {
			coalition->add(playerA);
		} catch (const std::exception& e) {
			exception = true;
		}
		CPPUNIT_ASSERT_EQUAL(true, exception);
		CPPUNIT_ASSERT_EQUAL(size_t(3), coalition->size());
		coalition->remove(playerA);
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerA));
		CPPUNIT_ASSERT_EQUAL(true, coalition->contains(playerB));
		CPPUNIT_ASSERT_EQUAL(true, coalition->contains(playerC));
		CPPUNIT_ASSERT_EQUAL(size_t(2), coalition->size());
		coalition->remove(playerB);
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerA));
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerB));
		CPPUNIT_ASSERT_EQUAL(true, coalition->contains(playerC));
		CPPUNIT_ASSERT_EQUAL(size_t(1), coalition->size());
		coalition->remove(playerC);
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerA));
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerB));
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerC));
		CPPUNIT_ASSERT_EQUAL(size_t(0), coalition->size());
		// Try to remove not-contained playerC.
		coalition->remove(playerC);
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerA));
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerB));
		CPPUNIT_ASSERT_EQUAL(false, coalition->contains(playerC));
		CPPUNIT_ASSERT_EQUAL(size_t(0), coalition->size());
	}
	
	void testCharacteristicFunction() {
		std::cout << "[ShapleyTests/testCharacteristicFunction]" << std::endl;
		TaxiCharacteristicFunction characteristicFunction;
		CPPUNIT_ASSERT_EQUAL(fareC, characteristicFunction.getValue(*coalition));
		coalition->remove(playerC);
		CPPUNIT_ASSERT_EQUAL(fareB, characteristicFunction.getValue(*coalition));
		coalition->remove(playerB);
		CPPUNIT_ASSERT_EQUAL(fareA, characteristicFunction.getValue(*coalition));
		coalition->remove(playerA);
		CPPUNIT_ASSERT_EQUAL(0.0, characteristicFunction.getValue(*coalition));
	}
	
	void testCoalitionUpUntil() {
		std::cout << "[ShapleyTests/testCoalitionUpUntil]" << std::endl;
		TaxiCharacteristicFunction characteristicFunction;
		CPPUNIT_ASSERT_EQUAL(fareC, characteristicFunction.getValue(coalition->getUpUntil(3)));
		CPPUNIT_ASSERT_EQUAL(fareB, characteristicFunction.getValue(coalition->getUpUntil(2)));
		CPPUNIT_ASSERT_EQUAL(fareA, characteristicFunction.getValue(coalition->getUpUntil(1)));
	}
	
	void testMarginalContribution() {
		std::cout << "[ShapleyTests/testMarginalContribution]" << std::endl;
		TaxiCharacteristicFunction charFunc;
		std::vector<const TaxiPlayer*> permutation;
		
		// (A,B,C)
		permutation.push_back(playerA);
		permutation.push_back(playerB);
		permutation.push_back(playerC);
		std::map<const TaxiPlayer*, double> marginalContributions = getMarginalContribution(permutation, charFunc);
		CPPUNIT_ASSERT_EQUAL(permutation.size(), marginalContributions.size());
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerA), 6.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerB), 6.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerC), 30.0);
		
		// (A,C,B)
		permutation.clear();
		permutation.push_back(playerA);
		permutation.push_back(playerC);
		permutation.push_back(playerB);
		marginalContributions = getMarginalContribution(permutation, charFunc);
		CPPUNIT_ASSERT_EQUAL(permutation.size(), marginalContributions.size());
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerA), 6.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerB), 0.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerC), 36.0);
		
		// (B,A,C)
		permutation.clear();
		permutation.push_back(playerB);
		permutation.push_back(playerA);
		permutation.push_back(playerC);
		marginalContributions = getMarginalContribution(permutation, charFunc);
		CPPUNIT_ASSERT_EQUAL(permutation.size(), marginalContributions.size());
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerA), 0.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerB), 12.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerC), 30.0);
		
		// (B,C,A)
		permutation.clear();
		permutation.push_back(playerB);
		permutation.push_back(playerC);
		permutation.push_back(playerA);
		marginalContributions = getMarginalContribution(permutation, charFunc);
		CPPUNIT_ASSERT_EQUAL(permutation.size(), marginalContributions.size());
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerA), 0.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerB), 12.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerC), 30.0);
		
		// (C,A,B)
		permutation.clear();
		permutation.push_back(playerC);
		permutation.push_back(playerA);
		permutation.push_back(playerB);
		marginalContributions = getMarginalContribution(permutation, charFunc);
		CPPUNIT_ASSERT_EQUAL(permutation.size(), marginalContributions.size());
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerA), 0.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerB), 0.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerC), 42.0);
		
		// (C,B,A)
		permutation.clear();
		permutation.push_back(playerC);
		permutation.push_back(playerB);
		permutation.push_back(playerA);
		marginalContributions = getMarginalContribution(permutation, charFunc);
		CPPUNIT_ASSERT_EQUAL(permutation.size(), marginalContributions.size());
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerA), 0.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerB), 0.0);
		CPPUNIT_ASSERT_EQUAL(marginalContributions.at(playerC), 42.0);
	}
	
	void testCompute() {
		std::cout << "[ShapleyTests/testCompute]" << std::endl;
		std::vector<const TaxiPlayer*> players;
		players.push_back(playerA);
		players.push_back(playerB);
		players.push_back(playerC);
		TaxiCharacteristicFunction charFunc;
		std::map<const TaxiPlayer*, double> shapleyValues = compute(players, charFunc);
		CPPUNIT_ASSERT_EQUAL(shapleyValues.at(playerA), 2.0);
		CPPUNIT_ASSERT_EQUAL(shapleyValues.at(playerB), 5.0);
		CPPUNIT_ASSERT_EQUAL(shapleyValues.at(playerC), 35.0);
	}
	
	CPPUNIT_TEST_SUITE(ShapleyTests);
		CPPUNIT_TEST(testCoalition);
		CPPUNIT_TEST(testCharacteristicFunction);
		CPPUNIT_TEST(testCoalitionUpUntil);
		CPPUNIT_TEST(testMarginalContribution);
		CPPUNIT_TEST(testCompute);
	CPPUNIT_TEST_SUITE_END();
};
