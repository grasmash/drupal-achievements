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
  $stats['first']  = $query->condition('rank', 5, '<=')->orderBy('rank')->range(0, 10)->execute()->fetchAllAssoc('rank');
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
      '#caption'    => t('!type achievement unlocks', array('!type' => drupal_ucfirst($type))),
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
    '#type'   => 'fieldset', 
    '#title'  => t('Clear cache'),
  );
  $form['achievements_cache']['achievements_clear_info_cache'] = array(
    '#type'   => 'submit', 
    '#value'  => t('Clear achievements information cache'),
    '#submit' => array('achievements_clear_info_cache_submit'),
  );

  return system_settings_form($form);
}

/**
 * Submit callback; clear achievement info cache.
 */
function achievements_clear_info_cache_submit() {
  achievements_load(NULL, TRUE);
  drupal_set_message('Achievements information cache cleared.');
}

