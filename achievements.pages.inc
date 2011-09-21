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
 * @param $top_count
 *   The number of top ranks to display.
 */
function achievements_leaderboard_totals($block = FALSE) {
  $header = array(t('#'), t('Who'), t('Points'), t('Unlocks'), t('When'));
  $header = $block ? array_slice($header, 0, 3) : $header; // UNIQUEZORS.
  !$block ? drupal_set_title(t('Achievement leaderboard')) : ''; // UNIQUEZORS+.

  // load up all our achievers for page or block.
  $query = db_select('achievement_totals', 'at');
  $query->addTag('achievement_totals')->join('users', 'u', 'u.uid = at.uid');
  $query->fields('at', array('uid', 'points', 'unlocks', 'timestamp'))->fields('u', array('name'));
  $query->orderBy('at.points', 'DESC')->orderBy('at.timestamp'); // @bug DESC/ASC doesn't index.
  $query = $block // blocks have a set count, but full page leaderboards have a pager.
    ? $query->range(0, variable_get('achievements_leaderboard_block_count_top', 5))
    : $query->extend('PagerDefault')->limit(variable_get('achievements_leaderboard_count_per_page', 25));
  $achievers = $query->execute()->fetchAllAssoc('uid');

  // display our achievers in "how awesome they are" order.
  // Global rank isn't stored in the database (to prevent us from having
  // complicated rejiggering every time an achievement is unlocked), but
  // the order the query returns is the order of the leaderboard. thus,
  // we start at 1 and increase the rank for every row we're showing.
  // also, be careful to start with the right number on pager queries.
  $rank = $block ? 1 : 1 + ($GLOBALS['pager_page_array'][0] * variable_get('achievements_leaderboard_count_per_page', 25));
  foreach ($achievers as $achiever) {
    $row = array(
      array(
        'data'  => $rank++,
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
      '#markup'   =>  l(t('View full leaderboard »'), 'achievements/leaderboard'),
      '#suffix'   => '</div>',
    );
  }
  else {
    $build['achievements']['leaderboard_pager'] = array(
      '#theme'    => 'pager',
    );
  }

  return $build;
}

/**
 * Menu callback; display a single achievement page with leaderboards.
 */
function achievements_leaderboard_for($achievement) {
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
  drupal_set_title(t('Achievements for @name', array('@name' => $account->name)));
  $unlocks = achievements_unlocked_already(NULL, $account->uid);
  $achiever = achievements_totals_user(0, $account->uid);

  $build['achievements']['stats'] = array(
    '#prefix' => '<div class="achievement-stats">',
    '#markup' => t('@name is ranked #@rank with @points points. @unlocks_count of @total_count achievements have been unlocked.', array(
      '@name'           => $account->name,
      '@rank'           => $achiever->rank,
      '@points'         => $achiever->points,
      '@unlocks_count'  => count($unlocks),
      '@total_count'    => count(achievements_load()))),
    '#suffix' => '</div>',
  );

  $achievements_grouped = achievements_load(NULL, TRUE);

  $build['achievements']['tabs']= array(
    '#prefix' => '<div id="achievement-groups">',
    '#suffix' => '</div>', // even without tabs.
  );

  // use jQuery tabs if more than one group.
  if (count($achievements_grouped) > 1) {
    foreach ($achievements_grouped as $group_id => $group) {
      $links['achievement-group-' . $group_id] = array(
        'title'     => $group['title'],
        'fragment'  => 'achievement-group-' . $group_id,
        'href'      => '', // just the fragment, please.
        'external'  => TRUE, // no base_path dammit. GZUS.
      );
    }
    $build['achievements']['tabs']['navigation'] = array('#theme' => 'links', '#links' => $links);
    $build['achievements']['tabs']['navigation']['#attached']['library'][] = array('system', 'ui.tabs');
    $build['achievements']['tabs']['navigation']['#attached']['js']= array( // I AM NOT A JS EXPERT. OOooOH NO.
      'jQuery(document).ready(function(){jQuery("#achievement-groups").tabs();});' => array('type' => 'inline')
    );
  }

  foreach ($achievements_grouped as $group_id => $group) {
    $locked_weight = 0; // we can't use definition order.
    $build['achievements']['tabs']['groups'][$group_id]['#prefix'] = '<div id="achievement-group-' . $group_id .'" class="achievement-group">';
    $build['achievements']['tabs']['groups'][$group_id]['#suffix'] = '</div>'; // for the jQuery UI tabs for when there's lotsa groups.

    foreach ($group['achievements'] as $achievement_id => $achievement) {
      $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#theme'] = 'achievement';
      $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#achievement'] = $achievement;

      if (isset($unlocks[$achievement_id])) {
        $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#unlock'] = $unlocks[$achievement_id];

        if (variable_get('achievements_unlocked_move_to_top', TRUE)) {
          $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#weight'] = -$unlocks[$achievement_id]->timestamp;
          // by setting the negative weight to the timestamp, the latest unlocks are always shown at the top.
        }
      }
      elseif (!isset($unlocks[$achievement_id]) && variable_get('achievements_unlocked_move_to_top', TRUE)) {
        // if we're forcing unlocks to the top, locked achievements have to be forced to the bottom too.
        $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#weight'] = $locked_weight++;
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
