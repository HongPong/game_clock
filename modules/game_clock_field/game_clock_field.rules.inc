<?php
// $Id$

/**
 * @file
 * Provides additional rules support for game_clock_field fields.
 */

/**
 * Implementation of hook_rules_action_info().
 */
function game_clock_field_rules_action_info() {
  $info = array();
  $info['game_clock_field_rules_action_load'] = array(
    'label' => t('Load a referenced game clock'),
    'arguments' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content containing the game clock field'),
      ),
    ),
    'new variables' => array(
      'referenced_game_clock' => array(
        'type' => 'game_clock',
        'label' => t('Referenced game clock'),
      ),
    ),
    'module' => 'CCK',
    'help' => t('Note that if the field has multiple values, only the first game clock will be loaded.'),
  );
  return $info;
}

function game_clock_field_rules_action_load($node, $settings) {
  if ($cid = $node->{$settings['field']}[0]['cid']) {
    return array('referenced_game_clock' => game_clock_state($cid, TRUE));
  }
}

function game_clock_field_rules_action_load_form($settings, &$form) {
  $settings += array('field' => '');
  $options = content_rules_get_field_names_by_type('game_clock_field');
  $form['settings']['field'] = array(
    '#type' => 'select',
    '#title' => t('Field'),
    '#default_value' => $settings['field'],
    '#options' => $options,
    '#required' => TRUE,
    '#disabled' => empty($options),
    '#description' => empty($options) ? t('There are no game_clock_field fields defined.') : '',
  );
}

/**
 * Helps upgrading from the workflow-ng action
 * "workflow_ng_action_load_referenced_game_clock" to the equivalent rules action.
 */
function workflow_ng_action_load_referenced_game_clock_upgrade(&$element) {
  $element['#name'] = 'game_clock_field_rules_action_load';
}
