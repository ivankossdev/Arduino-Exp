#pragma once
#include "input.h"

class Buttons : public IInput {
public:
  void          begin() override;
  IInput::Event poll() override;
  void          suppressUntilNeutral() override;

private:
  // --- MODE ---
  bool          m_modeRaw    = false;
  bool          m_modeStable = false;
  unsigned long m_modeChange = 0;

  // --- Направления ---
  int           m_dirRaw     = -1;
  unsigned long m_dirChange  = 0;
  bool          m_dirLocked  = false;
};