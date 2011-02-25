<?php

/**
 *  @file
 *  The administration page for the Game Clock module.
 */

/**
 *  Define the help pages for hook_help().
 */
function _game_clock_help($section) {
  switch ($section) {
    case 'admin/settings/game_clock':
      return t("
        <p>Game clocks may be used for various purposes, such as keeping an in-game calendar, tracking game effects and events, and limiting characters to acting at a slower pace suitable for a web-based game.</p>
        <p>You may view all active game clocks from this page, as well as create new clocks below. Additionally, you can create new game clocks programmatically with the API provided with the Game Clock module. See the !help page for more information.</p>
", array('!help' => l(t('Game Clock help'), 'admin/help/game_clock')));
    case 'admin/help#game_clock':
      return t("
        <p>Game clocks may be used for various purposes, such as keeping an in-game calendar, tracking game effects and events, and limiting characters to acting at a slower pace suitable for a web-based game.</p>
        <p>You may view all active game clocks from the !game_clock page, as well as create new clocks there. Additionally, you can create new game clocks programmatically with the API provided with the Game Clock module.</p>
        <p>To do so, you would create a game clock array as follows:</p>
<pre>  \$game = array(
    'name' => \$name,           // A unique machine-name.
    'title' => \$title,         // A human-readable title.
    'status' => \$status,       // If TRUE, then the clock will begin started. If FALSE it begins paused. Defaults to FALSE.
    'turn' => \$turn,           // The current turn to begin the clock. Defaults to 0.
    'increment' => \$increment, // How many seconds before incrementing to the next turn. Defaults to 0 (never; must be manually incremented).
    'block' => \$block,         // If TRUE, then a block displaying this clock's current turn and status will become available. Defaults to FALSE.
    'init' => \$init,           // If TRUE, then the clock will be checked for incrementation on every page load, assuming the proper time has passed.
  );
  game_clock_create(\$game);
</pre>
        <p>Other available functions in the Game Clock API that may be useful:</p>
<pre>
  game_clock_pause(\$clock = 'default', \$status = FALSE);
  game_clock_start(\$clock = 'default');
  game_clock_increment(\$clock = 'default', \$force = FALSE);
  game_clock_reset(\$clock = 'default', \$turn = NULL);
  game_clock_state(\$clock = NULL);
</pre>
        <p>Additionally, you may create a hook_game_clock function in your module to act on game clock events, as follows:</p>
<pre>
  function hook_game_clock(\$op, \$clock = 'default', \$state = NULL) {
    switch (\$op) {
      case 'create':
        // A clock named \$clock has been created with \$state.
        break;
      case 'pause':
        // The clock named \$clock has just been paused.
        break;
      case 'start':
        // The clock named \$clock has just been started.
        break;
      case 'increment':
        // The clock named \$clock has just been incremented a tick.
        break;
      case 'reset':
        // The clock named \$clock has just been reset (usually to 0).
        break;
    }
  }
</pre>
        <p>Please read the documentation in the game_clock.module file for more information.</p>
", array('!game_clock' => l(t('Game Clock administration'), 'admin/settings/game_clock')));
  }
}

function game_clock_settings_page() {
  $output .= drupal_get_form('game_clock_settings_form');
  $output .= drupal_get_form('game_clock_settings_add_form');
  return $output;
}

/**
 *  Callback for admin/settings/game_clock.
 */
function game_clock_settings_form() {
  $form = array();
  $options = array(
    'pause' => t('Pause game clocks'),
    'run' => t('Run game clocks'),
    'reset' => t('Reset game clocks'),
    'delete' => t('Delete game clocks'),
  );
  $form['selector'] = array(
    '#type' => 'select',
    '#title' => t('With selected game clocks:'),
    '#options' => $options,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form['#theme'] = 'game_clock_settings_form';
  $game_clocks = game_clock_state();
  $form['game_clocks'] = array(
    '#type' => 'value',
    '#value' => $game_clocks,
  );
  foreach ($game_clocks as $clock => $state) {
    $form['clocks'][$clock] = array();
    $form['clocks'][$clock]['selection'] = array(
      '#type' => 'checkbox',
    );
    $form['clocks'][$clock]['title'] = array(
      '#type' => 'item',
      '#value' => t('@title (@clock)', array('@title' => $state->title, '@clock' => $clock)),
    );
    $form['clocks'][$clock]['state'] = array(
      '#type' => 'item',
      '#value' => t('@state', array('@state' => ($state->status ? t('started') : t('paused')))),
    );
    $form['clocks'][$clock]['turn'] = array(
      '#type' => 'item',
      '#value' => t('@turn', array('@turn' => $state->turn)),
    );
    $form['clocks'][$clock]['increment'] = array(
      '#type' => 'item',
      '#value' => t('@increment', array('@increment' => _game_clock_selections($state->increment))),
    );
    $form['clocks'][$clock]['init'] = array(
      '#type' => 'item',
      '#value' => t('@init', array('@init' => ($state->init ? t('Yes') : t('No')))),
    );
    $ops = array(
      l(t('edit'), 'admin/settings/game_clock/'. $clock),
      l(t('pause'), 'admin/settings/game_clock/'. $clock .'/pause'),
      l(t('run'), 'admin/settings/game_clock/'. $clock .'/run'),
      l(t('reset'), 'admin/settings/game_clock/'. $clock .'/reset'),
      l(t('delete'), 'admin/settings/game_clock/'. $clock .'/delete'),
    );
    $form['clocks'][$clock]['ops'] = array(
      '#type' => 'item',
      '#value' => implode(', ', $ops),
    );
  }
  return $form;
}

function game_clock_settings_add_form() {
  $form = array();
  $form['clock'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create new game clock'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('You may add a new clock with this form. This can be useful for tracking game turns, special effects, and the like. You can also create clocks programmatically. See the !help page for more information.', array('!help' => l('game clock help', 'admin/help/game_clock'))),
  );
  $form['clock']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#description' => t('A unique machine name for this game clock. It must contain only lower-case, alphanumeric characters and underscores, and begin with a letter.'),
  );
  $form['clock']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('A human readable title for this game clock.'),
  );
  $form['clock']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Start clock'),
    '#default_value' => TRUE,
    '#description' => t('If checked, then this game clock will begin started. Otherwise, it will be paused.'),
  );
  $form['clock']['turn'] = array(
    '#type' => 'textfield',
    '#title' => t('Starting turn'),
    '#default_value' => '0',
    '#description' => t('What turn should the clock begin at?'),
  );
  $form['clock']['increment'] = array(
    '#type' => 'select',
    '#options' => _game_clock_selections(),
    '#default_value' => GAME_CLOCK_INCREMENT_DEFAULT,
    '#required' => TRUE,
    '#description' => t('The clock will wait at least this much time before incrementing to the next tick.'),
  );
  $form['clock']['block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create display block'),
    '#default_value' => TRUE,
    '#description' => t('If checked, then make a block available that will display the current turn and status of this clocked.'),
  );
  $form['clock']['init'] = array(
    '#type' => 'checkbox',
    '#title' => t('Increment clock on page load'),
    '#default_value' => TRUE,
    '#description' => 'If checked, then this clock will be checked for incrementation on normal page loads. Otherwise, it will only be checked during a cron run. If a clock has an increment value higher than the cron settings, it may be useful to check this box.',
  );
  $form['clock']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create clock'),
  );
  return $form;
}

function game_clock_settings_add_form_validate($form, &$form_state) {
  $state = $form_state['values'];
  // Ensure a safe machine name.
  if (!preg_match('/^[a-z_][a-z0-9_]*$/', $state['name'])) {
    form_set_error('name', t('The game clock name may only contain lowercase letters, underscores, and numbers; and begin with a letter.'));
  }
  else {
    // Ensure the machine name is unique.
    $test_state = game_clock_state($this['name']);
    if ($test_state->cid) {
      form_set_error('name', t('Game clock names must be unique. This game clock name is already in use.'));
    }
  }
}

function game_clock_settings_add_form_submit($form, &$form_state) {
  $state = game_clock_create($form_state['values'], TRUE);
  if ($state->cid) {
    drupal_set_message(t("The %clock game clock was successfully created.", array('%clock' => $state->title)));
  }
}

function _game_clock_selections($increment = NULL) {
  static $selections;
  if (is_null($selections)) {
    $selections = array(
      0 => t('Never'),
      1 => t('1 seconds'),
      2 => t('2 seconds'),
      3 => t('3 seconds'),
      5 => t('5 seconds'),
      15 => t('15 seconds'),
      30 => t('30 seconds'),
      60 => t('1 minute'),
      120 => t('2 minutes'),
      300 => t('5 minutes'),
      600 => t('10 minutes'),
      900 => t('15 minutes'),
      1800 => t('30 minutes'),
      3600 => t('1 hour'),
      (3600 * 2) => t('2 hours'),
      (3600 * 4) => t('4 hours'),
      (3600 * 8) => t('8 hours'),
      (3600 * 12) => t('12 hours'),
      (3600 * 24) => t('1 day'),
      (3600 * 24 * 2) => t('2 days'),
      (3600 * 24 * 3) => t('3 days'),
      (3600 * 24 * 4) => t('4 days'),
      (3600 * 24 * 5) => t('5 days'),
      (3600 * 24 * 6) => t('6 days'),
      (3600 * 24 * 7) => t('7 days'),
    );
  }
  if (is_null($increment)) {
    return $selections;
  }
  if (isset($selections[$increment])) {
    return $selections[$increment];
  }
  return t('@count seconds', array('@count' => $increment));
}

/**
 *  Create a new game clock.
 *  @param $state
 *    An object or array with at least the following neccessary parameters:
 *    ['name'] => A unique machine name.
 *    ['title'] => A human readable name for the clock.
 *    ['increment'] => The number of seconds before incrementing each tick.
 *  @param $report_errors
 *    If TRUE, then display messages of any errors. In any case, errors will be logged to the watchdog.
 *  @return
 *    Either the newly created clock object, or FALSE if there was an error.
 */
function _game_clock_create($state, $report_errors = FALSE) {
  $state = (object) $state;
  if (is_null($state->name)) {
    $error = 'Tried to create unnamed game clock.';
    if ($report_errors) {
      drupal_set_message(t($error), 'error');
    }
    watchdog('game_clock', $error, array(), WATCHDOG_ERROR);
    return FALSE;
  }
  $test_state = game_clock_state($state->name);
  if (!is_null($test_state)) {
    $error = 'Tried to create duplicate game clock: %clock.';
    $options = array('%clock' => $state->name);
    if ($report_errors) {
      drupal_set_message(t($error, $options), 'error');
    }
    watchdog('game_clock', $error, $options, WATCHDOG_ERROR);
    return FALSE;
  }
  $status = db_query("INSERT INTO {game_clocks} (name, title, status, turn, increment, block, init) VALUES ('%s', '%s', %d, %d, %d, %d, %d)", $state->name, $state->title, $state->status, $state->turn, $state->increment, $state->block, $state->init);
  if (!$status) {
    $error = 'Creation of the %clock game clock failed for an unknown reason.';
    if ($report_errors) {
      drupal_set_message($error, 'error');
    }
    watchdog('game_clock', $error, array(), WATCHDOG_ERROR);
    return FALSE;
  }
  $state = game_clock_state($state->name);
  module_invoke_all('game_clock', 'create', $state->name, $state);
  return $state;
}

/**
 *  Render the form as a table of clock settings and ops.
 */
function theme_game_clock_settings_form($form) {
  $output .= drupal_render($form['selector']);
  $output .= drupal_render($form['submit']);
  $select_header = theme('table_select_header_cell');
  $header = array($select_header, t('Title'), t('State'), t('Turn'), t('Increment'), t('On page load?'), t('Ops'));
  $rows = array();
  foreach ($form['game_clocks']['#value'] as $clock => $state) {
    $cells = array();
    $cells[] = drupal_render($form['clocks'][$clock]['selection']);
    $cells[] = drupal_render($form['clocks'][$clock]['title']);
    $cells[] = drupal_render($form['clocks'][$clock]['state']);
    $cells[] = drupal_render($form['clocks'][$clock]['turn']);
    $cells[] = drupal_render($form['clocks'][$clock]['increment']);
    $cells[] = drupal_render($form['clocks'][$clock]['init']);
    $cells[] = drupal_render($form['clocks'][$clock]['ops']);
    $rows[] = $cells;
  }
  $output .= theme('table', $header, $rows);
  return $output;
}
