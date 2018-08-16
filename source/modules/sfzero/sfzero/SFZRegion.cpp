/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/

#include "SFZRegion.h"
#include "SFZSample.h"

namespace sfzero
{

void EGParameters::clear()
{
  delay = 0.0;
  start = 0.0;
  attack = 0.0;
  hold = 0.0;
  decay = 0.0;
  sustain = 100.0;
  release = 0.0;
}

void EGParameters::clearMod()
{
  // Clear for velocity or other modification.
  delay = start = attack = hold = decay = sustain = release = 0.0;
}

Region::Region() { clear(); }

void Region::clear()
{
  memset(this, 0, sizeof(*this));
  hikey = 127;
  hivel = 127;
  pitch_keycenter = 60; // C4
  pitch_keytrack = 100;
  bend_up = 200;
  bend_down = -200;
  volume = pan = 0.0;
  amp_veltrack = 100.0;
  ampeg.clear();
  ampeg_veltrack.clearMod();
}

water::String Region::dump()
{
  water::String info = water::String::formatted("%d - %d, vel %d - %d", lokey, hikey, lovel, hivel);
  if (sample)
  {
    info << sample->getShortName();
  }
  info << "\n";
  return info;
}

float Region::timecents2Secs(int timecents) { return static_cast<float>(pow(2.0, timecents / 1200.0)); }

}
