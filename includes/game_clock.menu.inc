<?php
// $Id$

/**
 *  @file
 *  Menu definition for the Game Clock module.
 */

/**
 *  Helper function for game_clock_menu().
 */
function _game_clock_menu() {
  $items = array(
    'admin/settings/game_clock' => array(
      'title' => 'Game clocks',
      'description' => 'Administer the game clocks.',
      'page callback' => 'game_clock_settings_page',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
    ),
    'admin/settings/game_clock/list' => array(
      'title' => 'List',
      'description' => 'List the game clocks.',
      'page callback' => 'game_clock_settings_page',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_DEFAULT_LOCAL_TASK,
    ),
    'admin/settings/game_clock/add' => array(
      'title' => 'Add',
      'description' => 'Add a new game clock.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('game_clock_form'),
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 1,
    ),
    'admin/settings/game_clock/settings' => array(
      'title' => 'Settings',
      'description' => 'Configure the game clocks global settings.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('game_clock_configure_form'),
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 3,
    ),
    'admin/settings/game_clock/%game_clock' => array(
      'title' => 'Edit',
      'page callback' => 'game_clock_edit_page',
      'page arguments' => array(3),
      'description' => 'Edit a game clock.',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_CALLBACK,
    ),
    'admin/settings/game_clock/%game_clock/edit' => array(
      'title' => 'Edit',
      'page callback' => 'game_clock_edit_page',
      'page arguments' => array(3),
      'description' => 'Edit a game clock.',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -9,
    ),
    'admin/settings/game_clock/%game_clock/pause' => array(
      'title' => 'Pause',
      'page callback' => 'game_clock_pause_page',
      'page arguments' => array(3),
      'description' => 'Pause a game clock.',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => -7,
    ),
    'admin/settings/game_clock/%game_clock/start' => array(
      'title' => 'Run',
      'page callback' => 'game_clock_start_page',
      'page arguments' => array(3),
      'description' => 'Start a game clock.',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => -5,
    ),
    'admin/settings/game_clock/%game_clock/reset' => array(
      'title' => 'Reset',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('game_clock_reset_form', 3),
      'description' => 'Reset a game clock.',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => -3,
    ),
    'admin/settings/game_clock/%game_clock/delete' => array(
      'title' => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('game_clock_delete_form', 3),
      'description' => 'Delete a game clock.',
      'access arguments' => array('administer game clock'),
      'file' => 'includes/game_clock.admin.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => -1,
    ),
  );
  return $items;
}
