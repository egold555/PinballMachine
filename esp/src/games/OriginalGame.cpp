#include "OriginalGame.h"
#include "../sounds/Sounds.h"
#include <Preferences.h>

namespace
{
    const uint8_t SOLENOID_DELAY = 50;
    const uint8_t BALL_RETURN_PULSE = 100;
    const uint32_t BALL_KICK_RETRY_MS = 250;
    const uint32_t SPINNER_SCORE_DELAY = 75;

    long loadHighScore()
    {
        Preferences prefs;
        prefs.begin("original", true);
        long score = prefs.getLong("high", 0);
        prefs.end();
        return score;
    }
}

void OriginalGame::run(Machine &m)
{
    reset(m);
    attract(m);
    playGame(m);
}

// Reset the game back to the initial state.
void OriginalGame::reset(Machine &m)
{
    for (Player &p : players_)
        p = {true, true, true, true, 0};

    amountOfPlayers_ = 0;
    currentPlayer_ = 0;
    balls_ = 1;
    extraBalls_ = 0;
    bonus_ = 0;
    tilted_ = false;
    hasScoredThisRound_ = false;
    gameOver_ = false;

    for (LampId l : {LAMP_A, LAMP_B, LAMP_C, LAMP_D, LAMP_TARGET_LEFT, LAMP_TARGET_RIGHT, LAMP_TARGET_CENTER})
        m.setLamp(l, true);
    for (LampId l : {LAMP_BONUS_1000, LAMP_BONUS_2000, LAMP_BONUS_3000, LAMP_BONUS_4000, LAMP_BONUS_5000,
                     LAMP_BONUS_6000, LAMP_BONUS_7000, LAMP_BONUS_8000, LAMP_BONUS_9000, LAMP_BONUS_10000,
                     LAMP_EXTRA_BALL_RIGHT, LAMP_EXTRA_BALL_LEFT, LAMP_DOUBLE_BONUS, LAMP_TRIPLE_BONUS,
                     LAMP_SAME_PLAYER_SHOOTS, LAMP_BALL_1, LAMP_BALL_2, LAMP_BALL_3, LAMP_BALL_4, LAMP_BALL_5,
                     LAMP_PLAYER_1, LAMP_PLAYER_2, LAMP_PLAYER_3, LAMP_PLAYER_4})
        m.setLamp(l, false);

    m.setAutofire(0);
    m.playSound(SOUND_STARTUP);
}

// Scroll the attract message until start is pressed.
// Change from the original: other switches are ignored here. The original
// scored them (and could tilt) before the game had started.
void OriginalGame::attract(Machine &m)
{
    String msg = "PRESS START TO PLAY AGAIN           ";
    long high = loadHighScore();
    if (high > 0)
        msg += "HIGH SCORE - " + String(high) + "            ";
    m.scrollText(msg);

    SwitchEvent ev;
    while (!(m.nextEvent(ev, 1000) && ev.pressed && ev.id == SW_START))
    {
    }

    // startGame() from the original, state GS_BEFORE
    m.setLamp(LAMP_BALL_1, true);
    writeScore(m, 0, false);
    m.setAutofire(AUTOFIRE_ALL);
    addPlayer(m);
}

void OriginalGame::playGame(Machine &m)
{
    SwitchEvent ev;
    while (!gameOver_)
    {
        if (m.nextEvent(ev, 10))
        {
            if (ev.closed)
                handleSwitch(m, ev);
            continue;
        }

        // Nothing waiting: kick the ball out of the outhole if it is sitting
        // there (the original held the coil on while the switch was closed).
        if (m.isClosed(SW_BALL_RETURN) && (int32_t)(millis() - nextBallKick_) >= 0)
        {
            m.pulseCoil(COIL_BALL_RETURN, BALL_RETURN_PULSE);
            nextBallKick_ = millis() + BALL_KICK_RETRY_MS;
        }
    }
}

// One switch closing. This is updateScore() from the original.
void OriginalGame::handleSwitch(Machine &m, const SwitchEvent &ev)
{
    Player &player = players_[currentPlayer_];
    long oldScore = player.score;
    auto pressed = [&](SwitchId id) { return ev.pressed && ev.id == id; };

    if (pressed(SW_START))
        addPlayer(m);

    if (pressed(SW_BALL_RETURN))
    {
        // Not scored yet means the ball never made it to the playfield:
        // just kick it again instead of ending the ball.
        if (hasScoredThisRound_)
            endOfBall(m);
        return;
    }

    if (ev.id == SW_TILT && !tilted_)
    {
        tilt(m);
        return;
    }

    if (tilted_)
        return;

    // Thumper bumpers +100
    if (pressed(SW_LEFT_THUMPER_BUMPER) || pressed(SW_RIGHT_THUMPER_BUMPER))
        player.score += 100;

    // Spinners +100, at most once per SPINNER_SCORE_DELAY
    if (ev.id == SW_LEFT_SPINNER || ev.id == SW_RIGHT_SPINNER)
    {
        if ((int32_t)(ev.time - nextSpinnerScore_) >= 0)
        {
            player.score += 100;
            nextSpinnerScore_ = ev.time + SPINNER_SCORE_DELAY;
        }
    }

    // Extra ball lanes +500, and an extra ball when lit
    if (pressed(SW_LEFT_EXTRA_BALL_LANE) || pressed(SW_RIGHT_EXTRA_BALL_LANE))
        player.score += 500;

    if (pressed(SW_LEFT_EXTRA_BALL_LANE) && m.lamp(LAMP_EXTRA_BALL_LEFT))
    {
        extraBall(m);
        m.setLamp(LAMP_EXTRA_BALL_LEFT, false);
        m.blinkLamp(LAMP_EXTRA_BALL_LEFT, 100, 20);
    }

    if (pressed(SW_RIGHT_EXTRA_BALL_LANE) && m.lamp(LAMP_EXTRA_BALL_RIGHT))
    {
        extraBall(m);
        m.setLamp(LAMP_EXTRA_BALL_RIGHT, false);
        m.blinkLamp(LAMP_EXTRA_BALL_RIGHT, 100, 20);
    }

    // Advance lanes +500
    if (pressed(SW_LEFT_ADVANCE_LANE) || pressed(SW_RIGHT_ADVANCE_LANE))
    {
        advanceBonus();
        player.score += 500;
    }

    // ABCD +1000, and advance the bonus the first time each is hit
    const SwitchId abcdSwitches[] = {SW_A, SW_B, SW_C, SW_D};
    const LampId abcdLamps[] = {LAMP_A, LAMP_B, LAMP_C, LAMP_D};
    for (int i = 0; i < 4; i++)
    {
        if (pressed(abcdSwitches[i]))
        {
            player.score += 1000;
            if (m.lamp(abcdLamps[i]))
            {
                m.setLamp(abcdLamps[i], false);
                advanceBonus();
            }
        }
    }

    // All of ABCD collected: +25000 and light both extra ball lanes
    if (!m.lamp(LAMP_A) && !m.lamp(LAMP_B) && !m.lamp(LAMP_C) && !m.lamp(LAMP_D))
    {
        for (LampId l : abcdLamps)
            m.setLamp(l, true);
        player.score += 25000;
        m.setLamp(LAMP_EXTRA_BALL_LEFT, true);
        m.setLamp(LAMP_EXTRA_BALL_RIGHT, true);
        m.playSound(SOUND_STARTUP);
    }

    // Bumpers +50
    if (pressed(SW_LEFT_BUMPER) || pressed(SW_RIGHT_BUMPER))
        player.score += 50;

    // Targets +500, and the first hit on each advances the bonus twice
    if (pressed(SW_LEFT_TARGET) || pressed(SW_RIGHT_TARGET) || pressed(SW_CENTER_TARGET))
        player.score += 500;

    if (pressed(SW_LEFT_TARGET) && m.lamp(LAMP_TARGET_LEFT))
    {
        m.setLamp(LAMP_TARGET_LEFT, false);
        advanceBonus();
        advanceBonus();
    }

    if (pressed(SW_RIGHT_TARGET) && m.lamp(LAMP_TARGET_RIGHT))
    {
        m.setLamp(LAMP_TARGET_RIGHT, false);
        advanceBonus();
        advanceBonus();
    }

    if (pressed(SW_CENTER_TARGET) && m.lamp(LAMP_TARGET_CENTER))
    {
        player.score += 500; // the center target is worth 1000 when lit
        m.setLamp(LAMP_TARGET_CENTER, false);
        advanceBonus();
        advanceBonus();
    }

    // Targets 1 and 2 down: double bonus. All three: triple bonus.
    if (!m.lamp(LAMP_TARGET_LEFT) && !m.lamp(LAMP_TARGET_RIGHT))
    {
        if (!m.lamp(LAMP_TARGET_CENTER))
        {
            m.setLamp(LAMP_DOUBLE_BONUS, false);
            m.setLamp(LAMP_TRIPLE_BONUS, true);
        }
        else
        {
            m.setLamp(LAMP_DOUBLE_BONUS, true);
        }
    }

    const LampId bonusLamps[] = {LAMP_BONUS_1000, LAMP_BONUS_2000, LAMP_BONUS_3000, LAMP_BONUS_4000,
                                 LAMP_BONUS_5000, LAMP_BONUS_6000, LAMP_BONUS_7000, LAMP_BONUS_8000,
                                 LAMP_BONUS_9000, LAMP_BONUS_10000};
    for (int i = 0; i < bonus_; i++)
        m.setLamp(bonusLamps[i], true);

    if (oldScore != player.score)
    {
        writeScore(m, player.score);
        hasScoredThisRound_ = true;
        lightPlayerLights(m, currentPlayer_ + 1);
    }
}

// Start pressed during a game: add players until the first point is scored.
void OriginalGame::addPlayer(Machine &m)
{
    if (hasScoredThisRound_ || players_[0].score != 0)
        return;

    amountOfPlayers_++;
    if (amountOfPlayers_ > MAX_PLAYERS)
        amountOfPlayers_ = 1;
    m.playSound(SOUND_POINT, SoundPriority::Low);
    lightPlayerLights(m, amountOfPlayers_);
}

void OriginalGame::tilt(Machine &m)
{
    tilted_ = true;
    m.setAutofire(0);
    m.playSound(SOUND_TILT);
    m.showText("TILTED");
}

void OriginalGame::advanceBonus()
{
    if (bonus_ < 10)
        bonus_++;
}

void OriginalGame::extraBall(Machine &m)
{
    m.setLamp(LAMP_SAME_PLAYER_SHOOTS, true);
    extraBalls_++;
    m.playSound(SOUND_EXTRABALL);
}

// Change from the original: the scoring blip is Low priority, so it no longer
// cuts off the extra ball and ABCD sounds that are triggered by the same hit.
void OriginalGame::writeScore(Machine &m, long score, bool sound)
{
    m.showNumber(score);
    if (sound)
        m.playSound(SOUND_POINT, SoundPriority::Low);
}

// Light player lamp 1-4, or none for any other number.
void OriginalGame::lightPlayerLights(Machine &m, int num)
{
    m.setLamp(LAMP_PLAYER_1, num == 1);
    m.setLamp(LAMP_PLAYER_2, num == 2);
    m.setLamp(LAMP_PLAYER_3, num == 3);
    m.setLamp(LAMP_PLAYER_4, num == 4);
}

// No extra balls left: next player, and next ball after the last player.
void OriginalGame::switchPlayersOrAdvanceBall(Machine &m)
{
    Player &old = players_[currentPlayer_];
    old.lightA = m.lamp(LAMP_A);
    old.lightB = m.lamp(LAMP_B);
    old.lightC = m.lamp(LAMP_C);
    old.lightD = m.lamp(LAMP_D);

    if (++currentPlayer_ >= amountOfPlayers_)
    {
        balls_++;
        currentPlayer_ = 0;
    }

    Player &next = players_[currentPlayer_];
    m.setLamp(LAMP_A, next.lightA);
    m.setLamp(LAMP_B, next.lightB);
    m.setLamp(LAMP_C, next.lightC);
    m.setLamp(LAMP_D, next.lightD);
    lightPlayerLights(m, currentPlayer_ + 1);
}

// The ball drained: count the bonus, then the next ball or game over.
void OriginalGame::endOfBall(Machine &m)
{
    long bonusAmount = 0;
    m.playSound(SOUND_NEWBALL);

    if (!tilted_)
    {
        int multiplier = m.lamp(LAMP_DOUBLE_BONUS) ? 2 : m.lamp(LAMP_TRIPLE_BONUS) ? 3 : 1;
        bonusAmount = (long)bonus_ * 1000 * multiplier;
    }

    m.setLamp(LAMP_TARGET_LEFT, true);
    m.setLamp(LAMP_TARGET_RIGHT, true);
    m.setLamp(LAMP_TARGET_CENTER, true);

    bonus_ = 0;
    for (LampId l : {LAMP_BONUS_1000, LAMP_BONUS_2000, LAMP_BONUS_3000, LAMP_BONUS_4000, LAMP_BONUS_5000,
                     LAMP_BONUS_6000, LAMP_BONUS_7000, LAMP_BONUS_8000, LAMP_BONUS_9000, LAMP_BONUS_10000,
                     LAMP_EXTRA_BALL_LEFT, LAMP_EXTRA_BALL_RIGHT, LAMP_DOUBLE_BONUS, LAMP_TRIPLE_BONUS,
                     LAMP_SAME_PLAYER_SHOOTS, LAMP_BALL_1, LAMP_BALL_2, LAMP_BALL_3, LAMP_BALL_4, LAMP_BALL_5})
        m.setLamp(l, false);

    m.waitForSound();

    while (bonusAmount > 0)
    {
        m.sleep(500);
        bonusAmount -= 1000;
        players_[currentPlayer_].score += 1000;
        writeScore(m, players_[currentPlayer_].score);
    }

    m.sleep(1000);

    if (extraBalls_ == 0)
        switchPlayersOrAdvanceBall(m);
    else
        extraBalls_--;

    const LampId ballLamps[] = {LAMP_BALL_1, LAMP_BALL_2, LAMP_BALL_3, LAMP_BALL_4, LAMP_BALL_5};
    if (balls_ >= 1 && balls_ <= 5)
        m.setLamp(ballLamps[balls_ - 1], true);

    if (balls_ >= MAX_BALLS)
    {
        endGame(m);
        return;
    }

    hasScoredThisRound_ = false;
    m.pulseCoil(COIL_BALL_RETURN, BALL_RETURN_PULSE);
    nextBallKick_ = millis() + BALL_KICK_RETRY_MS;
    writeScore(m, players_[currentPlayer_].score);
    tilted_ = false;
    m.setAutofire(AUTOFIRE_ALL);
}

// Plays the ending song while showing each player's score and rank.
// Pressing start skips it.
void OriginalGame::endGame(Machine &m)
{
    gameOver_ = true;
    m.setAutofire(0);
    m.playSound(SOUND_ENDING_SONG);
    saveHighScore();

    for (LampId l : {LAMP_BALL_1, LAMP_BALL_2, LAMP_BALL_3, LAMP_BALL_4, LAMP_BALL_5})
        m.setLamp(l, false);
    for (LampId l : {LAMP_A, LAMP_B, LAMP_C, LAMP_D, LAMP_TARGET_LEFT, LAMP_TARGET_RIGHT, LAMP_TARGET_CENTER})
        m.setLamp(l, true);

    bool endNow = false;
    while (m.soundPlaying() && !endNow)
    {
        for (int i = 0; i < amountOfPlayers_ && !endNow; i++)
        {
            lightPlayerLights(m, i + 1);
            writeScore(m, players_[i].score, false);
            endNow = m.sleepUntilPressed(2000, SW_START);
            if (endNow)
                break;
            m.showText(rankingTitle(players_[i].score));
            endNow = m.sleepUntilPressed(2000, SW_START);
        }
        if (amountOfPlayers_ == 0)
            m.sleep(10);
    }

    m.stopSound();
    lightPlayerLights(m, 0);
}

void OriginalGame::saveHighScore()
{
    long best = 0;
    for (int i = 0; i < amountOfPlayers_; i++)
        best = max(best, players_[i].score);

    if (best > loadHighScore())
    {
        Preferences prefs;
        prefs.begin("original", false);
        prefs.putLong("high", best);
        prefs.end();
        Serial.printf("[game] new high score %ld\n", best);
    }
}

// Only 8 characters fit, so "FANTASTIC" shows as "FANTASTI", same as always.
const char *OriginalGame::rankingTitle(long score)
{
    if (score < 40000)
        return "AMATEUR";
    if (score < 75000)
        return "SUPER";
    if (score < 100000)
        return "CHAMP";
    if (score < 125000)
        return "FANTASTIC";
    if (score < 150000)
        return "WIZARD";
    return "FIREBALL";
}
