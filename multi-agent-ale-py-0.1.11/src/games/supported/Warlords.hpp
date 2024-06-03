/* *****************************************************************************
 * The lines 70 and 77 are based on Xitari's code, from Google Inc.
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

#ifndef __WARLORDS_HPP__
#define __WARLORDS_HPP__

#include "../RomSettings4P.hpp"

namespace ale {

/* RL wrapper for Fishing Derby settings */
class WarlordsSettings : public RomSettings4P {
 public:
  WarlordsSettings();

  // reset
  void reset() override;

  // is end of game
  bool isTerminal() const override;

  // get the most recently observed reward
  reward_t getReward() const override;
  reward_t getRewardP2() const override;
  reward_t getRewardP3() const override;
  reward_t getRewardP4() const override;

  // the rom-name
  const char* rom() const override { return "warlords"; }

  // The md5 checksum of the ROM that this game supports
  const char* md5() const override { return "cbe5a166550a8129a5e6d374901dffad"; }

  // create a new instance of the rom
  RomSettings* clone() const override;

  // is an action part of the minimal set?
  bool isMinimal(const Action& a) const override;

  // process the latest information from ALE
  void step(const System& system) override;

  // saves the state of the rom settings
  void saveState(Serializer& ser) override;

  // loads the state of the rom settings
  void loadState(Deserializer& ser) override;

  int lives() override;
  int livesP2() override;
  int livesP3() override;
  int livesP4() override;

  // returns a list of difficulties that the game can be played in
  // in this game, there are 4 available difficulties
  DifficultyVect getAvailableDifficulties() override;

  ModeVect getAvailableModes() override;
  ModeVect get2PlayerModes() override;
  ModeVect get4PlayerModes() override;

  void modifyEnvironmentSettings(Settings& settings);

  void setMode(game_mode_t, System& system,
               std::unique_ptr<StellaEnvironmentWrapper> environment) override;

 private:
  bool m_terminal;
  int m_lives[4];
  reward_t m_rewards[4];
  reward_t m_scores[4];
};

}  // namespace ale

#endif  // __WARLORDS_HPP__
