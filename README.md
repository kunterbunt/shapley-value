# shapley-value
C++ header-only implementation of the game theoretic Shapley value, including unit tests and usage example.

The [Shapley Value](https://en.wikipedia.org/wiki/Shapley_value) is a game theoretic concept that determines the fairest share of some resource among a number of players. An example would be the distribution of a taxi fare for three friends. That friend that contributes the most to the final taxi fare by living the farthest away, should also pay the largest share. A good example can be [viewed here](https://www.youtube.com/watch?v=aThG4YAFErw).

`shapley.h` provides the full implementation.   
To use it, you must implement 
- `Shapley::Player`, and provide a definition for `getContribution()`.
- `Shapley::CharacteristicFunction<YourPlayerType>`, and provide a definition for `getValue(Coalition<YourPlayerType>)`

An example for this can be found in `src/tests/ShapleyTests.cpp`, which is exactly the video explanation linked above.

Having done this, finding the fair division is a matter of

```
std::vector<const YourPlayerType*> players;
players.push_back(playerA);
players.push_back(playerB);
players.push_back(playerC);
YourCharacteristicFunction charFunc;
std::map<const YourPlayerType*, double> shapleyValues = compute(players, charFunc);
```

and you'll have a `<Player, ShapleyValue> map` at your disposal. :)
