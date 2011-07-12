<?php

/**
 * @file
 * Page callbacks for the Achievements module.
 */

/**
 * Menu callback and block; show the site-wide leaderboard.
 *
 * @param $block
 *   Defaults to FALSE; whether this is a block display.
 */
function achievements_leaderboard_totals($block = FALSE) {
  $achievers = achievements_totals($block ? 5 : 50);
  $header = array(t('#'), t('Who'), t('Points'), t('Unlocks'), t('When'));
  $header = $block ? array_slice($header, 0, 3) : $header; // UNIQUEZORS.
  !$block ? drupal_set_title(t('Achievement leaderboard')) : ''; // UNIQUEZORS+.
  drupal_add_css(drupal_get_path('module', 'achievements') . '/achievements.css');

  $rows = array();
  foreach ($achievers as $achiever) {
    $row = array( // dueling styles for wipe.
      array('data' => $achiever->rank, 'class' => array('achievement-leaderboard-rank')),
      array('data' => theme('username', array('account' => $achiever)), 'class' => array('achievement-leaderboard-username')),
      array('data' => l($achiever->points, 'user/' . $achiever->uid . '/achievements'), 'class' => array('achievement-leaderboard-points')),
    );

    if (!$block) { // full page display comes with a few more columns.
      $row[] = array('data' => l($achiever->unlocks, 'user/' . $achiever->uid . '/achievements'), 'class' => array('achievement-leaderboard-unlocks'));
      $row[] = array('data' => format_date($achiever->timestamp, 'small'), 'class' => array('achievement-leaderboard-when')); // mOOoOdooD for lOoovoe.
    }

    $rows[] = $row;
  }

  $render['achievements']['leaderboard'] = array(
    '#attributes' => array('class' => array('achievement-leaderboard')),
    '#header'     => $header, // 3 or 5 columns based on $block.
    '#empty'      => t('No one has been ranked yet.'),
    '#rows'       => $rows,
    '#theme'      => 'table',
  );

  if ($block) { // link off to more. MOOOReEeE!
    $render['achievements']['see-more'] = array(
      '#prefix'   => '<div class="achievement-see-more">',
      '#markup'   =>  l(t('View the top 50 leaders »'), 'achievements/leaderboard'),
      '#suffix'   => '</div>',
    );
  }

  return $render;
}

/**
 * Menu callback; display a single achievement page with leaderboards.
 */
function achievements_leaderboard_for($achievement) {
  drupal_add_css(drupal_get_path('module', 'achievements') . '/achievements.css');
  drupal_set_title(t('Achievement: @title', array('@title' => $achievement['title'])));

  $render['achievements']['achievement'] = array(
    '#theme'        => 'achievement',
    '#achievement'  => $achievement,
    '#unlock'       => achievements_unlocked_already($achievement['id']),
  );

  // get stats for first and most recent unlocks.
  $query = db_select('achievement_unlocks', 'au');
  $query->join('achievement_totals', 'at', 'at.uid = au.uid');
  $query->join('users', 'u', 'u.uid = au.uid'); // same basic start for both queries.
  $query->condition('achievement_id', $achievement['id']); // ... with a slight tweak.
  $query->fields('au', array('uid', 'rank', 'timestamp'))->fields('at', array('points', 'unlocks'))->fields('u', array('name'));
  $query2 = clone $query; // allows us to save a few lines of duplicate query building. never used clone before. awesome.
  $stats['first']  = $query->orderBy('rank')->range(0, 10)->execute()->fetchAllAssoc('rank'); // FI... sigh.
  $stats['recent'] = $query2->orderBy('timestamp', 'DESC')->range(0, 10)->execute()->fetchAllAssoc('rank');

  // both stat tables are displayed similarly.
  foreach (array('first', 'recent') as $type) {
    $rows = array(); // clear previous run.
    foreach ($stats[$type] as $stat) {
      $rows[] = array(
        array('data' => $stat->rank, 'class' => array('achievement-leaderboard-rank')),
        array('data' => theme('username', array('account' => $stat)), 'class' => array('achievement-leaderboard-username')),
        array('data' => l($stat->points, 'user/' . $stat->uid . '/achievements'), 'class' => array('achievement-leaderboard-points')),
        array('data' => format_date($stat->timestamp, 'short'), 'class' => array('achievement-leaderboard-when')),
      );
    }

    $render['achievements']['stats'][$type] = array(
      '#attributes' => array('class' => array('achievement-stats-' . $type)),
      '#caption'    => t('@type achievement unlocks', array('@type' => drupal_ucfirst($type))),
      '#header'     => array(t('#'), t('Who'), t('Points'), t('When')),
      '#empty'      => t('No one has unlocked this yet. Keep trying!'),
      '#rows'       => $rows,
      '#theme'      => 'table',
    );
  }

  return $render;
}

/**
 * Menu callback; display all achievements for the passed user.
 *
 * @param $account
 *   The user object this request applies against.
 */
function achievements_user_page($account) {
  drupal_add_css(drupal_get_path('module', 'achievements') . '/achievements.css');
  drupal_set_title(t('Achievements for @name', array('@name' => $account->name)));

  $all_achievements = achievements_load();
  $unlocks = db_select('achievement_unlocks', 'au')->fields('au', array('achievement_id', 'rank', 'timestamp'))
    ->condition('uid', $account->uid)->orderBy('timestamp', 'DESC')->execute()->fetchAllAssoc('achievement_id');

  $render['achievements']['stats'] = array(
    '#prefix' => '<div class="achievement-stats">',
    '#markup' => t('@name is ranked #@rank with @points points. @unlocks_count of @total_count achievements have been unlocked.', array(
      '@name'           => $account->name,
      '@rank'           => achievements_totals_user('rank', $account->uid),
      '@points'         => achievements_totals_user('points', $account->uid),
      '@unlocks_count'  => count($unlocks),
      '@total_count'    => count($all_achievements))),
    '#suffix' => '</div>',
  );
  foreach ($unlocks as $achievement_id => $unlock) {
    $render['achievements']['unlocks'][$achievement_id] = array(
      '#theme'        => 'achievement',
      '#achievement'  => $all_achievements[$achievement_id],
      '#unlock'       => (array)$unlock,
    );
  }

  return $render;
}

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
    '#title'              => t('Manual achievement overrides'),
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

  return $form;
}

/**
 * Submit callback; clear achievement info cache.
 */
function achievements_clear_info_cache_submit() {
  achievements_load(NULL, TRUE);
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

/**
 * Menu callback; Retrieve autocomplete suggestions for achievement names.
 */
function achievements_autocomplete($string = '') {
  $achievements = achievements_load(); // MmmMMmMm, gooOoaaAaalLLlsss.
  array_walk($achievements, 'achievements_autocomplete_search', $string);
  drupal_json_output(array_slice(array_filter($achievements), 0, 10));
}

/**
 * array_walk helper function for achievements_autocomplete().
 */
function achievements_autocomplete_search(&$value, $key, $string) {
  $value = (stripos($value['title'], $string) === FALSE) ? FALSE : $value['title'];
}

