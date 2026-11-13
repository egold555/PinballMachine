// The original rules, ported from the old Arduino code (Arduino/src/main.cpp).
// It should play exactly like the machine always has. Deliberate differences
// are marked "Change from the original".
#pragma once

#include "GameMode.h"

class OriginalGame : public GameMode
{
public:
    const char *name() const override { return "Original"; }
    void run(Machine &m) override;

private:
    struct Player
    {
        bool lightA, lightB, lightC, lightD;
        long score;
    };

    static const uint8_t MAX_PLAYERS = 4;
    static const uint8_t MAX_BALLS = 6; // starts at 1, so 5 balls

    void reset(Machine &m);
    void attract(Machine &m);
    void playGame(Machine &m);
    void handleSwitch(Machine &m, const SwitchEvent &ev);
    void addPlayer(Machine &m);
    void tilt(Machine &m);
    void advanceBonus();
    void extraBall(Machine &m);
    void writeScore(Machine &m, long score, bool sound = true);
    void lightPlayerLights(Machine &m, int num);
    void switchPlayersOrAdvanceBall(Machine &m);
    void endOfBall(Machine &m);
    void endGame(Machine &m);
    void saveHighScore();
    static const char *rankingTitle(long score);

    Player players_[MAX_PLAYERS];
    int amountOfPlayers_ = 0;
    int currentPlayer_ = 0;
    uint8_t balls_ = 1;
    uint8_t extraBalls_ = 0;
    uint8_t bonus_ = 0;
    bool tilted_ = false;
    bool hasScoredThisRound_ = false;
    bool gameOver_ = false;
    uint32_t nextSpinnerScore_ = 0;
    uint32_t nextBallKick_ = 0;
};
