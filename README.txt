
Game Clock

Initial development by Aaron Winborn.

Game clocks may be used for various purposes, such as keeping an in-game
calendar, tracking game effects and events, and limiting characters to acting
at a slower pace than might be suitable for a web-based game.

You may view all active game clocks from the Game Clock administration page,
as well as create new clocks there. Additionally, you can create new game
clocks programmatically with the API provided with the Game Clock module.

To do so, you would create a game clock array as follows:

  $game = array(
    'name' => $name,           // A unique machine-name.
    'title' => $title,         // A human-readable title.
    'status' => $status,       // If TRUE, then the clock will begin started.
                               // If FALSE it begins paused. Defaults to FALSE.
    'turn' => $turn,           // The current turn to begin the clock.
                               // Defaults to 0.
    'increment' => $increment, // How many seconds before incrementing to the
                               // next turn. Defaults to 0 (never; must be
                               // manually incremented).
    'block' => $block,         // If TRUE, then a block displaying this clock's
                               // current turn and status will become
                               // available. Defaults to FALSE.
    'init' => $init,           // If TRUE, then the clock will be checked for
                               // incrementation on every page load, assuming
                               // the proper time has passed.
  );
  game_clock_create($game);

Other available functions in the Game Clock API that may be useful:

  game_clock_pause($clock = 'default', $status = FALSE);
  game_clock_start($clock = 'default');
  game_clock_increment($clock = 'default', $force = FALSE);
  game_clock_reset($clock = 'default', $turn = NULL);
  game_clock_state($clock = NULL);

Additionally, you may create a hook_game_clock function in your module to act
on game clock events, as follows:

  function hook_game_clock($op, $clock = 'default', $state = NULL) {
    switch ($op) {
      case 'create':
        // A clock named $clock has been created with $state.
        break;
      case 'pause':
        // The clock named $clock has just been paused.
        break;
      case 'start':
        // The clock named $clock has just been started.
        break;
      case 'increment':
        // The clock named $clock has just been incremented a tick.
        break;
      case 'reset':
        // The clock named $clock has just been reset (usually to 0).
        break;
    }
  }

Please read the documentation in the game_clock.module file for more information.
