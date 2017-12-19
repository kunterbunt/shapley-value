//
// Created by Sebastian Lindner on 19.12.17.
//

#ifndef SHAPLEY_UNITTESTS_TUGAMESHAPLEYTESTS_H
#define SHAPLEY_UNITTESTS_TUGAMESHAPLEYTESTS_H

#include <algorithm>
#include "shapley.h"

class TUGamePlayer : public Shapley::Player {
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

class TUGameCharacteristicFunction : public Shapley::CharacteristicFunction<TUGamePlayer> {
  public:
	/**
	 * @param allPlayers The characteristic function for a given coalition needs to sum up the demands of all players *not* in that coalition, hence we need a reference to all players.
	 */
	TUGameCharacteristicFunction(Shapley::Coalition<TUGamePlayer>* allPlayers, double bandwidth)
			: allPlayers(allPlayers), availableBandwidth(bandwidth) {}
	
	/**
	 * @param coalition
	 * @return A coalition’s worth is what is left, if anything, once the players outside the coalition have received their own claims.
	 */
	double getValue(const Shapley::Coalition<TUGamePlayer>& coalition) const override {
		double aggregateDemand = 0.0;
		for (const TUGamePlayer* player : allPlayers->getMembers()) {
			if (!coalition.contains(player))
				aggregateDemand += player->getBandwidthDemand();
		}
		return std::max(0.0, availableBandwidth - aggregateDemand);
	}
  
  protected:
	const Shapley::Coalition<TUGamePlayer>* allPlayers = nullptr;
	double availableBandwidth = 0.0;
};

#endif //SHAPLEY_UNITTESTS_TUGAMESHAPLEYTESTS_H
