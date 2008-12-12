<?php
// $Id$

/**
 *  @file
 *  Theme functions for the Game Clock module.
 */

/**
 *  Prints the number of the current turn. If the game is paused, also indicates that.
 *  @param $clock
 *    The machine name of the game clock to display.
 */
function theme_game_clock_block($clock = 'default') {
  $state = game_clock_state($clock);
  $output = t('Turn: @turn', array('@turn' => $state->turn));
  if (!$state->status) {
    $output .= ' <span>'. t('(paused)') .'</span>';
  }
  return $output;
}
