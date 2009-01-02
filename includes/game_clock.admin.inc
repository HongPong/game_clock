<?php
// $Id$

/**
 *  @file
 *  The administration page for the Game Clock module.
 */

/**
 *  Callback for admin/settings/game_clock.
 */
function game_clock_list_page() {
  $output .= drupal_get_form('game_clock_list_form');
  global $game_clock_settings_page_standard_form;
  if ($game_clock_settings_page_standard_form) {
    $output .= drupal_get_form('game_clock_form');
    $output .= variable_get('game_clock_settings_form_count', 50) ? theme('pager') : '';
  }
  return $output;
}

/**
 *  Listing of game clocks on top of the admin/settings/game_clock page.
 */
function game_clock_list_form($form_state) {
  if (isset($form_state['values']['selector']) && $form_state['values']['selector'] == 'delete') {
    foreach ($form_state['values']['clocks'] as $clock => $selected) {
      if ($selected['selection']) {
        $items[$clock] = $form_state['values']['game_clocks'][$clock];
      }
    }
    return game_clock_delete_form($form_state, $items);
  }
  else if (isset($form_state['values']['selector']) && $form_state['values']['selector'] == 'reset') {
    foreach ($form_state['values']['clocks'] as $clock => $selected) {
      if ($selected['selection']) {
        $items[$clock] = $form_state['values']['game_clocks'][$clock];
      }
    }
    return game_clock_reset_form($form_state, $items);
  }

  // We are printing the standard form. That means we get an add form and pager.
  global $game_clock_settings_page_standard_form;
  $game_clock_settings_page_standard_form = TRUE;

  $game_clocks = array();
  $header = array(
    theme('table_select_header_cell'),
    array('data' => t('Title'), 'field' => 'title', 'sort' => 'asc'),
    array('data' => t('Type'), 'field' => 'type'),
    array('data' => t('Status'), 'field' => 'status'),
    array('data' => t('Turn'), 'field' => 'turn'),
    array('data' => t('Increment'), 'field' => 'increment'),
    array('data' => t('Page load'), 'field' => 'init'),
    t('Ops'));
  $sql = "SELECT * FROM {game_clocks}";
  $sql .= tablesort_sql($header);
  if (variable_get('game_clock_settings_form_count', 50)) {
    $results = pager_query($sql, variable_get('game_clock_settings_form_count', 50));
  }
  else {
    $results = db_query($sql);
  }
  while ($clock = db_fetch_object($results)) {
    $game_clocks[$clock->name] = $clock;
  }

  $form = array();
  $form['table_header'] = array(
    '#type' => 'value',
    '#value' => $header,
  );
  $options = array(
    'pause' => t('Pause game clocks'),
    'start' => t('Start game clocks'),
    'reset' => t('Reset game clocks'),
    'delete' => t('Delete game clocks'),
  );
  $form['selector'] = array(
    '#type' => 'select',
    '#title' => t('With selected game clocks'),
    '#options' => $options,
  );
  $form['#theme'] = 'game_clock_settings_form';
  $form['game_clocks'] = array(
    '#type' => 'value',
    '#value' => $game_clocks,
  );
  $form['clocks'] = array(
    '#tree' => TRUE,
  );
  foreach ($game_clocks as $clock => $state) {
    $form['clocks'][$clock] = array();
    $form['clocks'][$clock]['selection'] = array(
      '#type' => 'checkbox',
    );
    $form['clocks'][$clock]['title'] = array(
      '#type' => 'item',
      '#value' => l(t('@title', array('@title' => $state->title)), 'admin/settings/game_clock/'. $clock),
    );
    $form['clocks'][$clock]['type'] = array(
      '#type' => 'item',
      '#value' => t('@type', array('@type' => $state->type)),
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
      l(t('start'), 'admin/settings/game_clock/'. $clock .'/start'),
      l(t('reset'), 'admin/settings/game_clock/'. $clock .'/reset'),
      l(t('delete'), 'admin/settings/game_clock/'. $clock .'/delete'),
    );
    $form['clocks'][$clock]['ops'] = array(
      '#type' => 'item',
      '#value' => implode(', ', $ops),
    );
    $form['help'] = array(
      '#type' => 'item',
      '#value' => t('If a game clock has been started, it will increment a turn every time the increment time passes. If %init is set to Yes, then the clock will check for incrementation as appropriate on user page loads. Otherwise, it will check only during the cron run.', array('%init' => t('Page load'))),
    );
    $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  }
  return $form;
}

function game_clock_list_form_submit($form, &$form_state) {
  $clocks = array();
  foreach ($form_state['values']['clocks'] as $clock => $value) {
    if ($value['selection']) {
      $clocks[] = $clock;
    }
  }
  if (!empty($clocks)) {
    switch ($form_state['values']['selector']) {
      case 'delete':
      case 'reset':
        $form_state['rebuild'] = TRUE;
        break;
      case 'pause':
        foreach ($clocks as $clock) {
          game_clock_pause($clock);
          drupal_set_message(t("Paused the %clock game clock.", array('%clock' => $form_state['values']['game_clocks'][$clock]->title)));
        }
        break;
      case 'start':
        foreach ($clocks as $clock) {
          game_clock_start($clock);
          drupal_set_message(t("Started the %clock game clock.", array('%clock' => $form_state['values']['game_clocks'][$clock]->title)));
        }
        break;
    }
  }
}

/**
 *  Delete confirmation form.
 */
function game_clock_delete_form(&$form_state, $clocks) {
  if (is_object($clocks)) {
    $title = t('Are you sure you wish to delete the %clock game clock?', array('%clock' => $clocks->title));
    $message = t('If you press %delete, then the %clock game clock will be deleted, and may not be recovered.', array('%delete' => t('Delete'), '%clock' => $clocks->title));
    $clocks = array($clocks->name => $clocks);
  }
  else if (is_array($clocks) && !empty($clocks)) {
    $title = t('Are you sure you wish to delete these game clocks?');
    $message = t('If you press %delete, then the following game clocks will be deleted:', array('%delete' => t('Delete')));
    $items = array();
    foreach ($clocks as $state) {
      $items[] = t('%clock', array('%clock' => $state->title));
    }
    $message .= theme('item_list', $items);
    $message .= t('These game clocks may not be recovered.');
  }
  else {
    drupal_not_found();
  }
  $form['clocks'] = array(
    '#type' => 'value',
    '#value' => $clocks,
  );
  $form['#submit'][] = 'game_clock_delete_form_submit';
  return confirm_form($form, $title, 'admin/settings/game_clock', $message, t('Delete'));
}

/**
 *  Delete the submitted game clock(s).
 */
function game_clock_delete_form_submit($form, &$form_state) {
  foreach ($form_state['values']['clocks'] as $clock => $state) {
    game_clock_delete($clock, TRUE);
  }
  $form_state['redirect'] = 'admin/settings/game_clock';
}

/**
 *  Reset confirmation form.
 */
function game_clock_reset_form(&$form_state, $clocks) {
  if (is_object($clocks)) {
    $title = t('Are you sure you wish to reset the %clock game clock?', array('%clock' => $clocks->title));
    $message = t('If you press %reset, then the %clock game clock will be reset to 0 turns, which may not be undone.', array('%reset' => t('Reset'), '%clock' => $clocks->title));
    $clocks = array($clocks->name => $clocks);
  }
  else if (is_array($clocks) && !empty($clocks)) {
    $title = t('Are you sure you wish to reset these game clocks?');
    $message = t('If you press %reset, then the following game clocks will be reset to 0 turns:', array('%reset' => t('Reset')));
    $items = array();
    foreach ($clocks as $state) {
      $items[] = t('%clock', array('%clock' => $state->title));
    }
    $message .= theme('item_list', $items);
    $message .= t('This action may not be undone.');
  }
  else {
    drupal_not_found();
  }
  $form['clocks'] = array(
    '#type' => 'value',
    '#value' => $clocks,
  );
  $form['#submit'][] = 'game_clock_reset_form_submit';
  return confirm_form($form, $title, 'admin/settings/game_clock', $message, t('Reset'));
}

/**
 *  Reset the submitted game clock(s).
 */
function game_clock_reset_form_submit($form, &$form_state) {
  foreach ($form_state['values']['clocks'] as $clock => $state) {
    game_clock_reset($clock);
  }
  $form_state['redirect'] = 'admin/settings/game_clock';
}

/**
 *  Callback for admin/settings/game_clock/%game_clock.
 *  This displays the edit game clock form.
 */
function game_clock_edit_page($clock) {
  if (!$clock->cid) {
    drupal_not_found();
  }
  drupal_set_title(t('Edit @clock', array('@clock' => $clock->title)));
  return drupal_get_form('game_clock_form', $clock);
}

/**
 *  Callback for admin/settings/game_clock/%game_clock/pause.
 *  This pauses the game clock.
 */
function game_clock_pause_page($clock) {
  game_clock_pause($clock->name);
  drupal_set_message(t("Paused the %clock game clock.", array('%clock' => $clock->title)));
  drupal_goto('admin/settings/game_clock');
}

/**
 *  Callback for admin/settings/game_clock/%game_clock/start.
 *  This pauses the game clock.
 */
function game_clock_start_page($clock) {
  game_clock_start($clock->name);
  drupal_set_message(t("Started the %clock game clock.", array('%clock' => $clock->title)));
  drupal_goto('admin/settings/game_clock');
}

/**
 *  Define the form to create a new game clock.
 */
function game_clock_form($form_state, $clock = NULL) {
  $clock = (object) $clock;
  $form = array();
  $form['clock'] = array(
    '#type' => 'fieldset',
    '#title' => !isset($clock->cid) ? t('Create new game clock') : t('Game clock'),
    '#collapsible' => !isset($clock->cid) && (arg(3) != 'add'),
    '#collapsed' => !isset($clock->cid) && (arg(3) != 'add'),
    '#description' => t('Game clocks can be useful for tracking game turns, special effects, and the like. You can also create clocks programmatically. See the !help page for more information.', array('!help' => l('game clock help', 'admin/help/game_clock'))),
  );
  $form['clock']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#description' => t('A unique machine name for this game clock. It must contain only lower-case, alphanumeric characters and underscores, and begin with a letter.'),
    '#default_value' => isset($clock->name) ? $clock->name : '',
  );
  $form['clock']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('A human readable title for this game clock.'),
    '#default_value' => isset($clock->title) ? $clock->title : '',
    '#required' => TRUE,
  );
  $form['clock']['type'] = array(
    '#type' => 'textfield',
    '#title' => t('Type'),
    '#description' => t('The optional type of the game clock. This is useful for sorting and filtering game clocks on the listing page. Additionally, some modules may act on or affect only clocks of this type.'),
    '#default_value' => isset($clock->type) ? $clock->type : '',
  );
  $form['clock']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Start clock'),
    '#description' => t('If checked, then this game clock will begin started. Otherwise, it will be paused.'),
    '#default_value' => isset($clock->status) ? $clock->status : TRUE,
  );
  $form['clock']['turn'] = array(
    '#type' => 'textfield',
    '#title' => t('Starting turn'),
    '#default_value' => isset($clock->turn) ? $clock->turn : 0,
    '#description' => t('What turn should the clock begin at?'),
  );
  $form['clock']['increment'] = array(
    '#type' => 'select',
    '#title' => t('Increment'),
    '#options' => _game_clock_selections(),
    '#default_value' => isset($clock->increment) ? $clock->increment : variable_get('game_clock_increment_default', 5),
    '#required' => TRUE,
    '#description' => t('The clock will wait at least this much time before incrementing to the next tick.'),
  );
  $form['clock']['block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create display block'),
    '#default_value' => isset($clock->block) ? $clock->block : FALSE,
    '#description' => t('If checked, then make a block available that will display the current turn and status of this clocked.'),
  );
  $form['clock']['init'] = array(
    '#type' => 'checkbox',
    '#title' => t('Increment clock on page load'),
    '#default_value' => isset($clock->init) ? $clock->init : TRUE,
    '#description' => 'If checked, then this clock will be checked for incrementation on normal page loads. Otherwise, it will only be checked during a cron run. If a clock has an increment value higher than the cron settings, it may be useful to check this box.',
  );
  $form['clock']['submit'] = array(
    '#type' => 'submit',
    '#value' => isset($clock->cid) ? t('Save changes') : t('Create clock'),
  );
  $form['clock']['cid'] = array(
    '#type' => 'value',
    '#value' => $clock->cid,
  );
  return $form;
}

/**
 *  Callback form for admin/settings/game_clock/settings.
 */
function game_clock_settings() {
  // This creates a list of the number of clocks to limit.
  $options = array(0 => 'All', 1 => 1, 5 => 5, 10 => 10, 25 => 25, 50 => 50, 100 => 100, 250 => 250, 500 => 500, 1000 => 1000);
  $form = array();
  $form['game_clock_settings_form_count'] = array(
    '#type' => 'select',
    '#title' => t('Settings listing form clock count'),
    '#description' => t('This displays how many clocks to display on the game clock settings listing form page.'),
    '#options' => $options,
    '#default_value' => variable_get('game_clock_settings_form_count', 50),
  );
  $form['game_clock_increment_default'] = array(
    '#type' => 'select',
    '#title' => t('Increment default'),
    '#description' => t('This determines the default increment value for a newly created clock. This value may easily be set when creating clocks, and is simply provided as a convenience for administrators and developers who need to create a large number of similar clocks.'),
    '#options' => _game_clock_selections(),
    '#default_value' => variable_get('game_clock_increment_default', 5),
  );
  $form['game_clock_limit_on_init'] = array(
    '#type' => 'select',
    '#title' => t('Page load limit'),
    '#description' => t('This limits how many clocks to process during a single page load. Set it if you have a high volume of game clocks and have pages timing out.'),
    '#options' => $options,
    '#default_value' => variable_get('game_clock_limit_on_init', 0),
  );
  $form['game_clock_limit_on_cron'] = array(
    '#type' => 'select',
    '#title' => t('Cron limit'),
    '#description' => t('This limits how many clocks to process during each cron run. Set it if you have a high volume of game clocks that never seem to run.'),
    '#options' => $options,
    '#default_value' => variable_get('game_clock_limit_on_cron', 0),
  );
  return system_settings_form($form);
}

/**
 *  Form validation for game_clock_form.
 *  @see game_clock_form
 */
function game_clock_form_validate($form, &$form_state) {
  $state = $form_state['values'];
  // Ensure a safe machine name.
  if (!preg_match('/^[a-z_][a-z0-9_]*$/', $state['name'])) {
    form_set_error('name', t('The game clock name may only contain lowercase letters, underscores, and numbers; and begin with a letter.'));
  }
  else {
    // Ensure the machine name is unique.
    $test_state = game_clock_state($state['name']);
    if ($test_state->cid && ($test_state->cid != $state['cid'])) {
      form_set_error('name', t('Game clock names must be unique. This game clock name is already in use.'));
    }
  }
  if (isset($state['cid'])) {
    $test_state = game_clock_state($state['cid'], TRUE);
    if (!isset($test_state->cid)) {
      form_set_error('', t('This game clock has been deleted.'));
    }
  }
}

/**
 *  Form submission for game_clock_form.
 *  @see game_clock_form
 */
function game_clock_form_submit($form, &$form_state) {
  if (isset($form_state['values']['cid'])) {
    // Get the currently stored state.
    $state = game_clock_state($form_state['values']['cid'], TRUE);

    // Store the old name, so we can redirect if it's changed.
    $old_name = $state->name;

    // Only save if the state is already in the database.
    // This should already be taken care of in the validation step.
    if ($state->cid) {
      // By calling array_merge, we replace the $state with the new values.
      $state = array_merge((array) $state, $form_state['values']);

      // Save our new game clock state.
      $state = game_clock_save($state);

      drupal_set_message(t("Saved changes to the %clock game clock.", array('%clock' => $state->title)));

      // If we changed the name, then redirect to the new edit page.
      if ($state && ($old_name != $state->name)) {
        drupal_goto('admin/settings/game_clock/'. $state->name);
      }
    }
  }
  else {
    // Create a new game clock state.
    $state = game_clock_create($form_state['values'], TRUE);
  }
}

/**
 *  Build an array of increment times, suitable for a select drop-down list.
 *  @param $increment
 *    (optional) The number of seconds for which we wish to see a description.
 *  @return
 *    If $increment is NULL, then return the entire array.
 *    If it's a key in the array, then return the resulting description.
 *    Otherwise, return a translated string of '@increment seconds'.
 */
function _game_clock_selections($increment = NULL) {
  static $selections;
  if (is_null($selections)) {
    $selections = array(
      0 => t('Never'),
      1 => t('1 seconds'),
      2 => t('2 seconds'),
      3 => t('3 seconds'),
      4 => t('4 seconds'),
      5 => t('5 seconds'),
      10 => t('10 seconds'),
      15 => t('15 seconds'),
      30 => t('30 seconds'),
      60 => t('1 minute'),
      120 => t('2 minutes'),
      180 => t('3 minutes'),
      240 => t('4 minutes'),
      300 => t('5 minutes'),
      600 => t('10 minutes'),
      900 => t('15 minutes'),
      1800 => t('30 minutes'),
      3600 => t('1 hour'),
      (3600 * 2) => t('2 hours'),
      (3600 * 3) => t('3 hours'),
      (3600 * 4) => t('4 hours'),
      (3600 * 5) => t('5 hours'),
      (3600 * 6) => t('6 hours'),
      (3600 * 8) => t('8 hours'),
      (3600 * 10) => t('10 hours'),
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
  return t('@increment seconds', array('@increment' => $increment));
}

/**
 *  Render the form as a table of clock settings and ops.
 */
function theme_game_clock_settings_form($form) {
  $output .= drupal_render($form['selector']);
  $header = $form['table_header']['#value'];
  $rows = array();
  foreach ($form['game_clocks']['#value'] as $clock => $state) {
    $cells = array();
    $cells[] = drupal_render($form['clocks'][$clock]['selection']);
    $cells[] = drupal_render($form['clocks'][$clock]['title']);
    $cells[] = drupal_render($form['clocks'][$clock]['type']);
    $cells[] = drupal_render($form['clocks'][$clock]['state']);
    $cells[] = drupal_render($form['clocks'][$clock]['turn']);
    $cells[] = drupal_render($form['clocks'][$clock]['increment']);
    $cells[] = drupal_render($form['clocks'][$clock]['init']);
    $cells[] = drupal_render($form['clocks'][$clock]['ops']);
    $rows[] = $cells;
  }
  $output .= theme('table', $header, $rows);
  $output .= drupal_render($form);

  return $output;
}

/**
 *  Return an array of all current game clock types.
 *  @param $force
 *    If TRUE, then force the types to rebuild from an otherwise static cache.
 */
function _game_clock_types($force = FALSE) {
  static $types;

  if (is_null($types) || $force) {
    $types = array();
    $results = db_query("SELECT type FROM {game_clocks} GROUP BY type");
    while ($state = db_fetch_object($results)) {
      $types[$state->type] = $state->type;
    }
  }
  return $types;
}
