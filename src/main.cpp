#include <game.h>
/// The main function, called first on execution.
///
/// @param argc The number of command line arguments
/// @param argv The values of the command line arguments
int main(int argc, char** argv)
{
  // Construct a game and initialise it
  Game mainGame;
  mainGame.run();
  return 0;
}
