<?php

/**
 * @file
 * Allow diff'ing Drush makefiles.
 */

/**
 * Implements hook_drush_command().
 */
function make_diff_drush_command() {
  $items['make-diff'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Show the differences between two makefiles.',
    'arguments' => array(
      'OldMakefile' => 'An outdated Drush makefile.',
      'NewMakefile' => 'An updated Drush makefile.',
    ),
    'options' => array(
      'list' => 'Output a simple list of projects.',
    ),
    'required-arguments' => TRUE,
    'aliases' => array('diff'),
  );

  return $items;
}

/**
 * Command callback. Diff two makefiles.
 */
function drush_make_diff($old_makefile, $new_makefile) {
  // Load, parse and validate our makefiles;
  $old_info = make_parse_info_file($old_makefile);
  $new_info = make_parse_info_file($new_makefile);
  if (!make_validate_info_file($old_info) || !make_validate_info_file($new_info)) {
    return FALSE;
  }
  // Filter down to just projects and versions
  $diff = array('header' => array('Project','Old version','New version'));
  foreach ($old_info['projects'] as $name => $info) {
    $diff[$name]['name'] = $name;
    $diff[$name]['old'] = $info['version'];
  }
  foreach ($new_info['projects'] as $name => $info) {
    // Ignore packages with the same version
    if (isset($diff[$name]) && $info['version'] == $diff[$name]['old']) {
      unset($diff[$name]);
      continue;
    }
    $diff[$name]['new'] = $info['version'];
    if ($info['version'] < $diff[$name]['old']) {
      $diff[$name]['notes'] = 'WARNING: version downgrade!';
    }
  }

  if (drush_get_option('list', TRUE)) {
    unset($diff['header']);
    $new = array();
    foreach ($diff as $name => $info) {
      $new[] = $name . '-7.x-' . $info['new'];
    }
    return implode($new, ' ');
  }
  else {
    return drush_format_table($diff, TRUE);
  }
}
