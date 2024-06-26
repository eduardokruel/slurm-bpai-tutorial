/* *****************************************************************************
 * The method lives() is based on Xitari's code, from Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details.
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 */

#include "WizardOfWor.hpp"

#include "../RomUtils.hpp"

namespace ale {
#include "stdio.h"

WizardOfWorSettings::WizardOfWorSettings() { reset(); }

/* create a new instance of the rom */
RomSettings* WizardOfWorSettings::clone() const {
  return new WizardOfWorSettings(*this);
}

/* process the latest information from ALE */
void WizardOfWorSettings::step(const System& system) {
  // update the reward
  reward_t score = getDecimalScore(0x86, 0x88, &system);
  if (score >= 8000)
    score -= 8000; // MGB score does not go beyond 999
  score *= 100;
  m_reward = score - m_score;
  m_score = score;

  reward_t scoreP2 = getDecimalScore(0x85, 0x87, &system);
  if (scoreP2 >= 8000)
    scoreP2 -= 8000; // MGB score does not go beyond 999
  scoreP2 *= 100;
  m_reward_p2 = scoreP2 - m_score_p2;
  m_score_p2 = scoreP2;

  // update terminal status
  int newLives = (readRam(&system, 0x8D) & 15) - 1;
  int newLivesP2 = (readRam(&system, 0x8C) & 15) - 1;
  int byte1 = readRam(&system, 0xF4);

  bool isWaiting = (readRam(&system, 0xD7) & 0x1) == 0;
  bool isWaitingP2 = (readRam(&system, 0xD6) & 0x1) == 0;

  if (m_reward == 1000 && m_lives_p2 > newLivesP2) {
    m_reward += 1;
  }

  // Check if player 2 killed player 1
  if (m_reward_p2 == 1000 && m_lives > newLives) {
    m_reward_p2 += 1;
  }

  m_terminal = newLives == -1 && byte1 == 0xF8 && (newLivesP2 == -1 || !is_two_player);

  // Wizard of Wor decreases the life total when we move into the play field; we only
  // change the life total when we actually are waiting
  m_lives = isWaiting ? newLives : m_lives;
  m_lives_p2 = isWaitingP2 ? newLivesP2 : m_lives_p2;
}

/* is end of game */
bool WizardOfWorSettings::isTerminal() const { return m_terminal; };

/* get the most recently observed reward */
reward_t WizardOfWorSettings::getReward() const { return m_reward; }
reward_t WizardOfWorSettings::getRewardP2() const { return m_reward_p2; }

int WizardOfWorSettings::lives() { return m_lives; }
int WizardOfWorSettings::livesP2() { return m_lives_p2; }

/* is an action part of the minimal set? */
bool WizardOfWorSettings::isMinimal(const Action& a) const {
  switch (a) {
  //  case PLAYER_A_NOOP:
    case PLAYER_A_FIRE:
    case PLAYER_A_UP:
    case PLAYER_A_RIGHT:
    case PLAYER_A_LEFT:
    case PLAYER_A_DOWN:
    case PLAYER_A_UPFIRE:
    case PLAYER_A_RIGHTFIRE:
    case PLAYER_A_LEFTFIRE:
    case PLAYER_A_DOWNFIRE:
      return true;
    default:
      return false;
  }
}

/* reset the state of the game */
void WizardOfWorSettings::reset() {
  m_reward = 0;
  m_reward_p2 = 0;
  m_score = 0;
  m_score_p2 = 0;
  m_terminal = false;
  m_lives = 2;
  m_lives_p2 = 2;
}

/* saves the state of the rom settings */
void WizardOfWorSettings::saveState(Serializer& ser) {
  ser.putInt(m_reward);
  ser.putInt(m_reward_p2);
  ser.putInt(m_score);
  ser.putInt(m_score_p2);
  ser.putBool(m_terminal);
  ser.putBool(is_two_player);
  ser.putInt(m_lives);
  ser.putInt(m_lives_p2);
}

// loads the state of the rom settings
void WizardOfWorSettings::loadState(Deserializer& ser) {
  m_reward = ser.getInt();
  m_reward_p2 = ser.getInt();
  m_score = ser.getInt();
  m_score_p2 = ser.getInt();
  m_terminal = ser.getBool();
  is_two_player = ser.getBool();
  m_lives = ser.getInt();
  m_lives_p2 = ser.getInt();
}

DifficultyVect WizardOfWorSettings::getAvailableDifficulties() {
  return {0, 1};
}

ModeVect WizardOfWorSettings::getAvailableModes()  {
  return {0};
}
ModeVect WizardOfWorSettings::get2PlayerModes()  {
  return {1};
}

// Set the game mode.
// The given mode must be one returned by the previous function.
void WizardOfWorSettings::setMode(
    game_mode_t m, System& system,
    std::unique_ptr<StellaEnvironmentWrapper> environment) {

  while (getDecimalScore(0x81, &system) != m) { environment->pressSelect(1); }

  is_two_player = (m == 1);

  // reset the environment to apply changes.
  environment->softReset();
}


}  // namespace ale
