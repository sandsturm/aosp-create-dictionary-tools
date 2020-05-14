#include "threads.h"

Threads::Threads(){
  while (run){

    // workers.push_back(std::thread(m_Spellcheck->load_spellcheckfile, std::ref(spellcheck)));
  }
}

void Threads::stop(){
  run = false;
}
