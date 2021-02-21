#include "functions.h"

int calculate_queue(int character, int workers){
  if(character < 0){
    return 0;
  } else if(character > 90){
    return (character - 97) * workers / 27;
  } else {
    return (character - 65) * workers / 27;
  }
}
