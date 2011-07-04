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
  $num_ranks = $block ? 5 : 50;
  $totals = achievements_totals_info();
  $displayed_ranks = array_slice($totals['by_rank'], 0, $num_ranks);
  $header = array(t('#'), t('Who'), t('Points'), t('Unlocks'), t('When'));
  $header = $block ? array_slice($header, 0, 3) : $header; // UNIQUEZORS.
  !$block ? drupal_set_title(t('Achievement leaderboard')) : ''; // UNIQUEZORS+.
  drupal_add_css(drupal_get_path('module', 'achievements') . '/achievements.css');

  // if the current user isn't in the top $num_ranks, show where they are. appease the downtrodden.
  if (user_is_logged_in() && isset($totals['by_user'][$GLOBALS['user']->uid]) && $totals['by_user'][$GLOBALS['user']->uid]['rank'] > $num_ranks) {
    $displayed_ranks[] = $totals['by_user'][$GLOBALS['user']->uid];
  }

  $rows = array();
  foreach ($displayed_ranks as $displayed_rank) {
    $row = array( // dueling styles for wipe.
      array('data' => $displayed_rank['rank'], 'class' => array('achievement-leaderboard-rank')),
      array('data' => theme('username', array('account' => (object)$displayed_rank)), 'class' => array('achievement-leaderboard-username')),
      array('data' => l($displayed_rank['points'], 'user/' . $displayed_rank['uid'] . '/achievements'), 'class' => array('achievement-leaderboard-points')),
    );

    if (!$block) { // full page display comes with a few more columns.
      $row[] = array('data' => l($displayed_rank['unlocks'], 'user/' . $displayed_rank['uid'] . '/achievements'), 'class' => array('achievement-leaderboard-unlocks'));
      $row[] = array('data' => format_date($displayed_rank['timestamp'], 'small'), 'class' => array('achievement-leaderboard-when')); // mOOoOdooD for lOoovoe.
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
  $query->join('users', 'u', 'u.uid = au.uid'); // same basic query for both stats.
  $query->fields('au', array('uid', 'rank', 'timestamp'))->fields('u', array('name'));
  $query->condition('achievement_id', $achievement['id']); // ... with a slight tweak.
  $stats['first'] = $query->condition('rank', 5, '<=')->orderBy('rank')->range(0, 10)->execute()->fetchAllAssoc('rank');
  array_pop($query->conditions()); $order =& $query->getOrderBy(); $order = NULL; // trickery to save us a few duplicate lines.
  $stats['recent'] = $query->condition('rank', 1, '>=')->orderBy('timestamp', 'DESC')->range(0, 10)->execute()->fetchAllAssoc('rank');

  // both stat tables are displayed similarly.
  foreach (array('first', 'recent') as $type) {
    $rows = array(); // clear previous run.
    foreach ($stats[$type] as $stat) {
      $rows[] = array(
        array('data' => $stat->rank, 'class' => array('achievement-leaderboard-rank')),
        array('data' => theme('username', array('account' => $stat)), 'class' => array('achievement-leaderboard-username')),
        array('data' => l(achievements_totals_info($stat->uid, 'points'), 'user/' . $stat->uid . '/achievements'), 'class' => array('achievement-leaderboard-points')),
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
      '@rank'           => achievements_totals_info($account->uid, 'rank'),
      '@points'         => achievements_totals_info($account->uid, 'points'),
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

