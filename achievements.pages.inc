<?php

/**
 * @file
 * Page callbacks for the Achievements module.
 */

/**
 * Menu callback and block; show the site-wide leaderboard.
 *
 * @param $block
 *   Whether this is a block display (defaults to FALSE).
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
      array(
        'data'  => $achiever->rank,
        'class' => array('achievement-leaderboard-rank')
      ),
      array(
        'data'  => theme('username', array('account' => $achiever)),
        'class' => array('achievement-leaderboard-username')
      ),
      array(
        'data'  => l($achiever->points, 'user/' . $achiever->uid . '/achievements'),
        'class' => array('achievement-leaderboard-points')
      ),
    );

    if (!$block) {
      $row[] = array(
        'data'  => l($achiever->unlocks, 'user/' . $achiever->uid . '/achievements'),
        'class' => array('achievement-leaderboard-unlocks')
      );
      $row[] = array(
        'data'  => format_date($achiever->timestamp, 'small'),
        'class' => array('achievement-leaderboard-when')
      );
    }

    $rows[] = $row;
  }

  $build['achievements']['leaderboard'] = array(
    '#attributes' => array('class' => array('achievement-leaderboard')),
    '#header'     => $header, // 3 or 5 columns based on $block.
    '#empty'      => t('No one has been ranked yet.'),
    '#rows'       => $rows,
    '#theme'      => 'table',
  );

  if ($block) { // link off to more. MOOOReEeE!
    $build['achievements']['see-more'] = array(
      '#prefix'   => '<div class="achievement-see-more">',
      '#markup'   =>  l(t('View the top 50 leaders »'), 'achievements/leaderboard'),
      '#suffix'   => '</div>',
    );
  }

  return $build;
}

/**
 * Menu callback; display a single achievement page with leaderboards.
 */
function achievements_leaderboard_for($achievement) {
  drupal_add_css(drupal_get_path('module', 'achievements') . '/achievements.css');
  drupal_set_title(t('Achievement: @title', array('@title' => $achievement['title'])));

  $build['achievements']['achievement'] = array(
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
        array(
          'data'  => $stat->rank,
          'class' => array('achievement-leaderboard-rank')
        ),
        array(
          'data'  => theme('username', array('account' => $stat)),
          'class' => array('achievement-leaderboard-username')
        ),
        array(
          'data'  => l($stat->points, 'user/' . $stat->uid . '/achievements'),
          'class' => array('achievement-leaderboard-points')
        ),
        array(
          'data'  => format_date($stat->timestamp, 'short'),
          'class' => array('achievement-leaderboard-when')
        ),
      );
    }

    $build['achievements']['stats'][$type] = array(
      '#attributes' => array('class' => array('achievement-stats-' . $type)),
      '#caption'    => t('@type achievement unlocks', array('@type' => drupal_ucfirst($type))),
      '#header'     => array(t('#'), t('Who'), t('Points'), t('When')),
      '#empty'      => t('No one has unlocked this yet. Keep trying!'),
      '#rows'       => $rows,
      '#theme'      => 'table',
    );
  }

  return $build;
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

  $achievements_grouped = achievements_load(NULL, TRUE);
  $unlocks = db_select('achievement_unlocks', 'au')->fields('au', array('achievement_id', 'rank', 'timestamp'))
    ->condition('uid', $account->uid)->orderBy('timestamp', 'DESC')->execute()->fetchAllAssoc('achievement_id');

  $build['achievements']['stats'] = array(
    '#prefix' => '<div class="achievement-stats">',
    '#markup' => t('@name is ranked #@rank with @points points. @unlocks_count of @total_count achievements have been unlocked.', array(
      '@name'           => $account->name,
      '@rank'           => achievements_totals_user('rank', $account->uid),
      '@points'         => achievements_totals_user('points', $account->uid),
      '@unlocks_count'  => count($unlocks), // non-grouped achievements.
      '@total_count'    => count(achievements_load()))),
    '#suffix' => '</div>',
  );

  foreach ($achievements_grouped as $group_id => $group) {
    $locked_weight = 0; // we can't use definition order.
    $build['achievements'][$group_id]['#prefix'] = '<div class="achievement-group-' . $group_id .'">';
    $build['achievements'][$group_id]['#suffix'] = '</div>'; // necessary for the jQuery UI tabs.

    foreach ($group['achievements'] as $achievement_id => $achievement) {
      $build['achievements'][$group_id][$achievement_id]['#theme'] = 'achievement';
      $build['achievements'][$group_id][$achievement_id]['#achievement'] = $achievement;

      if (isset($unlocks[$achievement_id])) {
        $build['achievements'][$group_id][$achievement_id]['#unlock'] = (array) $unlocks[$achievement_id];
        $build['achievements'][$group_id][$achievement_id]['#weight'] = -$unlocks[$achievement_id]->timestamp;
        // by setting the negative weight to the timestamp, the latest unlocks are always shown at the top.
      }
      else {
        // since we're forcing unlocked achievements to the top with #weight,
        // locked achievements can no longer depend on "definition order".
        $build['achievements'][$group_id][$achievement_id]['#weight'] = $locked_weight++;
      }
    }
  }

  return $build;
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
