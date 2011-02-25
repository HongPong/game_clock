<?php

/**
 *  Helper functions for the Game Clock API.
 */

/**
 *  Create a new game clock.
 *  @param $state
 *    An object or array with at least the following neccessary parameters:
 *    ['name'] => A unique machine name.
 *    ['title'] => A human readable name for the clock.
 *    ['increment'] => The number of seconds before incrementing each tick.
 *  @param $report_errors
 *    If TRUE, then display messages of any errors. In any case, errors will be
 *    logged to the watchdog.
 *  @return
 *    Either the newly created clock object, or FALSE if there was an error.
 */
function _game_clock_create($state, $report_errors = FALSE) {
  $state = (object) $state;
  if (is_null($state->name) || ("{$state->name}" === '')) {
    $error = 'Tried to create unnamed game clock.';
    game_clock_report_error($message, array(), $report_errors, 'error');
    return FALSE;
  }
  $test_state = game_clock_state($state->name);
  if (!is_null($test_state)) {
    $error = 'Tried to create duplicate game clock: %clock.';
    $options = array('%clock' => $state->name);
    game_clock_report_error($error, $options, $report_errors, 'error');
    return FALSE;
  }
  $status = db_query("INSERT INTO {game_clocks} (name, type, title, status, turn, increment, block, init) VALUES ('%s', '%s', '%s', %d, %d, %d, %d, %d)", $state->name, $state->type, $state->title, $state->status, $state->turn, $state->increment, $state->block, $state->init);
  if (!$status) {
    $error = 'Creation of the %clock game clock failed for an unknown reason.';
    $options = array('%clock' => $state->title);
    game_clock_report_error($error, $options, $report_errors, 'error');
    return FALSE;
  }
  $state = game_clock_state($state->name);
  module_invoke_all('game_clock_create', $state->name, $state);
  $message = 'Created %title game clock.';
  $options = array('%title' => $state->title);
  $link = l(t('@name', array('@name' => $state->title)), 'admin/settings/game_clock/'. $state->name);
  game_clock_report_error($message, $options, $report_errors, 'status', $link);
  return $state;
}

/**
 *  Delete a game clock.
 *  This will also invoke hook_game_clock_delete($state->name, $state),
 *  after it's been deleted from the database and wiped from the global
 *  variable.
 *  @param $clock
 *    The string of the clock.
 *  @param $report_errors
 *    If TRUE, then display messages of any errors. In any case, errors will be
 *    logged to the watchdog.
 *  @return
 *    TRUE if succeeded; otherwise, FALSE.
 */
function _game_clock_delete($clock, $report_errors = FALSE) {
  $state = game_clock_state($clock);
  if (!$state->cid) {
    $error = "Tried to delete non-existent game clock: %clock";
    $options = array('%clock' => $clock);
    game_clock_report_error($error, $options, $report_errors, 'error');
    return FALSE;
  }
  db_query("DELETE FROM {game_clocks} WHERE cid = %d", $state->cid);
  global $game_clocks;
  unset($game_clocks[$state->name]);
  module_invoke_all('game_clock_delete', $state->name, $state);
  $message = 'Deleted %title game clock.';
  $options = array('%title' => $state->title);
  game_clock_report_error($message, $options, $report_errors);
  return TRUE;
}

/**
 *  Saves a game clock state.
 *  This assumes the game clock has already been created, and is being edited.
 *  @param $state
 *    The game clock state to save.
 *  @param $report_errors
 *    If TRUE, then display messages of any errors. In any case, errors will be
 *    logged to the watchdog.
 *  @return
 *    The new state if succeeded; otherwise, FALSE.
 */
function _game_clock_save($state, $report_errors = FALSE) {
  $state = (object) $state;

  // As we may have changed our increment, ensure it starts over the timer.
  // Otherwise, you may have had a clock increment every 24 hours, then change
  // it to 15 seconds, but have to wait for a day before the first tick.
  $state->next_tick = 0;
  if ($state->cid) {
    // Make sure we're trying to save a clock that hasn't been deleted.
    $test_state = db_fetch_object(db_query("SELECT * FROM {game_clocks} WHERE cid = %d", $state->cid));
    if ($test_state->cid) {
      $status = db_query("UPDATE {game_clocks} SET name = '%s', type = '%s', title = '%s', status = %d, turn = %d, increment = %d, block = %d, init = %d, next_tick = 0 WHERE cid = %d", $state->name, $state->type, $state->title, $state->status, $state->turn, $state->increment, $state->block, $state->init, $state->cid);
      if (!$status) {
        $error = 'Saving the %clock game clock failed for an unknown reason.';
        $options = array('%clock' => $state->title);
        $link = l(t('edit'), 'admin/settings/game_clock/'. $state->name);
        game_clock_report_error($error, $options, $report_errors, 'error', $link);
        return FALSE;
      }
      global $game_clocks;
      $game_clocks[$state->name] = $state;
      module_invoke_all('game_clock_save', $state->name, $state);
      return $state;
    }
  }
  $error = 'Tried to save unknown game clock %name (with a CID of %cid).';
  $options = array('%name' => $state->name, '%cid' => $state->cid);
  game_clock_report_error($error, $options, $report_errors, 'error');
  return FALSE;
}

/**
 *  Report an error with a game clock function.
 *  @param $message
 *    The untranslated message to report.
 *  @param $options
 *    (optional) An array of any translating options to send, as in the
 *    t($message, $options) function.
 *  @param $report_errors
 *    (optional) If TRUE, then the error will be reported to the user via
 *    drupal_set_message.
 *    It will be logged through the watchdog in any case.
 *  @param $type
 *    (optional) The message type, either 'error' or 'status'.
 *  @param $link
 *    (optional) A link to pass to the watchdog.
 */
function game_clock_report_error($message, $options = array(), $report_errors = FALSE, $type = 'status', $link = NULL) {
  $watchdog_type = ($type == 'error') ? WATCHDOG_ERROR : WATCHDOG_NOTICE;
  if ($report_errors) {
    drupal_set_message(t($message, $options), $type);
  }
  watchdog('game_clock', $message, $options, $watchdog_type, $link);
}
