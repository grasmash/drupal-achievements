<?php

/**
 * @file
 * Admin callbacks for the Achievements module.
 */

/**
 * Configure achievements.
 */
function achievements_settings() {
  $form['achievements_cache'] = array(
    '#type'               => 'fieldset',
    '#title'              => t('Clear cache'),
  );
  $form['achievements_cache']['achievements_clear_info_cache'] = array(
    '#type'               => 'submit', // just like performance cache clears.
    '#submit'             => array('achievements_clear_info_cache_submit'),
    '#value'              => t('Clear achievements information cache'),
  );

  $form['achievements_manual'] = array(
    '#description'        => t("Taking an achievement from a user will <em>not</em> change other users' unlock ranking. <strong>This functionality should be used sparingly for progress-based achievements as it can create odd inconsistencies in the internal statistics.</strong> For example, giving a user the \"125 comments posted\" achievement does <em>not</em> mean they've actually posted 125 comments - if they've only made 15 legitimate comments, they'll still need to post 235 more to achieve \"250 comments posted\". Similarly, taking away an achievement will delete any relevant internal statistics, causing the user to \"start over\" with that achievement even if applicable and countable data remains."),
    '#title'              => t('Manual overrides'),
    '#type'               => 'fieldset',
  );
  $form['achievements_manual']['achievements_manual_user'] = array(
    '#autocomplete_path'  => 'user/autocomplete',
    '#maxlength'          => '60',
    '#title'              => t('Username'),
    '#type'               => 'textfield',
  );
  $form['achievements_manual']['achievements_manual_achievement'] = array(
    '#autocomplete_path'  => 'achievements/autocomplete',
    '#title'              => t('Achievement ID'),
    '#type'               => 'textfield',
  );
  $form['achievements_manual']['actions'] = array('#type' => 'actions');
  $form['achievements_manual']['actions']['achievements_manual_give'] = array(
    '#achievement_action' => 'give',
    '#type'               => 'submit',
    '#submit'             => array('achievements_manual_submit'),
    '#value'              => t('Give achievement'),
  );
  $form['achievements_manual']['actions']['achievements_manual_take'] = array(
    '#achievement_action' => 'take',
    '#type'               => 'submit',
    '#submit'             => array('achievements_manual_submit'),
    '#value'              => t('Take achievement'),
  );

  $form['achievements_rankings'] = array(
    '#title'              => t('Leaderboard ranks'),
    '#type'               => 'fieldset',
  );
  $form['achievements_rankings']['achievements_leaderboard_count_per_page'] = array(
    '#type'           => 'select',
    '#title'          => t('Number of top ranks per page'),
    '#default_value'  => variable_get('achievements_leaderboard_count_per_page', 25),
    '#options'        => drupal_map_assoc(array(5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100)),
  );

  $form['achievements_config'] = array(
    '#title'              => t('Additional configuration'),
    '#type'               => 'fieldset',
  );
  $module_path = drupal_get_path('module', 'achievements');
  foreach (array('unlocked', 'locked', 'hidden') as $image_type) {
    $form['achievements_config']['achievements_image_' . $image_type] = array(
      '#default_value'      => variable_get('achievements_image_' . $image_type, $module_path . '/images/default-' . $image_type . '-70.jpg'),
      '#title'              => t('Default @image_type picture', array('@image_type' => $image_type)),
      '#type'               => 'textfield',
    );
  }
  $form['achievements_config']['achievements_unlocked_move_to_top'] = array(
    '#type'           => 'checkbox',
    '#title'          => t("Move the user's unlocked achievements to the top of their achievement page."),
    '#default_value'  => variable_get('achievements_unlocked_move_to_top', TRUE), // default to gamer-style.
  );

  return system_settings_form($form);
}

/**
 * Submit callback; clear achievement info cache.
 */
function achievements_clear_info_cache_submit() {
  achievements_load(NULL, FALSE, TRUE);
  drupal_set_message(t('Achievements information cache cleared.'));
}

/**
 * Submit callback; give or take away an achievement from a user.
 */
function achievements_manual_submit($form, &$form_state) {
  $account     = user_load_by_name($form_state['values']['achievements_manual_user']);
  $achievement = achievements_load($form_state['values']['achievements_manual_achievement']);

  if (!$account) { // dear sir, you are quite unable...
    drupal_set_message(t('%username is not a valid user.',
      array('%username' => $form_state['values']['achievements_manual_user'])), 'error');
  }

  if (!$achievement) { // ...to do anything right! --the mgmt.
    drupal_set_message(t('%achievement_id is not a valid achievement.',
      array('%achievement_id' => $form_state['values']['achievements_manual_achievement'])), 'error');
  }

  if ($account && $achievement) {
    if ($form_state['clicked_button']['#achievement_action'] == 'give') {
      achievements_unlocked($achievement['id'], $account->uid);
      drupal_set_message(t('%username has been given %achievement.', // yeah, yeah.
        array('%username' => $account->name, '%achievement' => $achievement['title'])));
    }
    if ($form_state['clicked_button']['#achievement_action'] == 'take') {
      db_delete('achievement_unlocks')->condition('achievement_id', $achievement['id'])->condition('uid', $account->uid)->execute();
      db_update('achievement_totals') // remove from unlocks and then subtract from the current totals.
        ->fields(array(
          'uid'       => $account->uid,
          'timestamp' => REQUEST_TIME,
        ))
        ->expression('points', 'points - :points', array(':points' => $achievement['points']))
        ->expression('unlocks', 'unlocks - :increment', array(':increment' => 1))
        ->condition('uid', $account->uid)
        ->execute();

      // remove any internal statistic saved for this achievement.
      $storage = isset($achievement['storage']) ? $achievement['storage'] : $achievement['id'];
      db_delete('achievement_storage')->condition('achievement_id', $storage)->condition('uid', $account->uid)->execute();

      // and let 'em know the nasty deed has been done.
      drupal_set_message(t('%username has had %achievement taken away.', // nope, nope.
        array('%username' => $account->name, '%achievement' => $achievement['title'])));
    }
  }
}
