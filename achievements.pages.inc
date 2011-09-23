<?php

/**
 * @file
 * Page callbacks for the Achievements module.
 */

/**
 * Menu callback; show the site-wide leaderboard.
 *
 * @see achievement_leaderboard_block()
 */
function achievements_leaderboard_totals() {
  // force no sorting on # so it doesn't get the 'active' CSS, which screws up relative non-header display.
  $header = array(array('data' => t('#'), 'sort' => NULL), t('Who'), t('Points'), t('Unlocks'), t('When'));
  $achievers = $rows = array('top' => array(), 'relative' => array());

  // load up all our achievers for the page.
  $query = db_select('achievement_totals', 'at')->extend('PagerDefault');
  $query->addTag('achievement_totals')->join('users', 'u', 'u.uid = at.uid');
  $query->fields('at', array('uid', 'points', 'unlocks', 'timestamp'))->fields('u', array('name'));
  $query->orderBy('at.points', 'DESC')->orderBy('at.timestamp'); // @bug DESC/ASC doesn't index.
  $query->limit(variable_get('achievements_leaderboard_count_per_page', 25));
  $achievers['top'] = $query->execute()->fetchAllAssoc('uid');

  if (user_is_logged_in() && variable_get('achievements_leaderboard_relative', 'nearby_ranks') != 'disabled') {
    $achievers['relative'] = (variable_get('achievements_leaderboard_relative', 'nearby_ranks') == 'nearby_ranks')
      ? achievements_totals_user(variable_get('achievements_leaderboard_relative_nearby_ranks', 2))
      : achievements_totals_user(); // you and me, togetheRrrr FoReEEvvvVeeEeeRRrRR.
  }

  // display our achievers in "how awesome they are" order.
  // Global rank isn't stored in the database (to prevent us from having
  // complicated rejiggering every time an achievement is unlocked), but
  // the order the query returns is the order of the leaderboard. thus,
  // we start at 1 and increase the rank for every row we're showing.
  // also, be careful to start with the right number on pager queries.
  $rank = 1 + ($GLOBALS['pager_page_array'][0] * variable_get('achievements_leaderboard_count_per_page', 25));
  foreach (array('top', 'relative') as $type) {
    foreach ($achievers[$type] as $achiever) {
      $rows[$type][] = array(
        array(
          'data'  => isset($achiever->rank) ? $achiever->rank : $rank++,
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
        array(
          'data'  => l($achiever->unlocks, 'user/' . $achiever->uid . '/achievements'),
          'class' => array('achievement-leaderboard-unlocks')
        ),
        array(
          'data'  => format_date($achiever->timestamp, 'small'),
          'class' => array('achievement-leaderboard-when')
        ),
      );
    }
  }

  foreach (array('top', 'relative') as $type) {
    $build['achievements']['leaderboard'][$type] = array(
      '#attributes' => array('class' => array('achievement-leaderboard', 'achievement-leaderboard-' . $type)),
      '#header'     => $type == 'top' ? $header : NULL, // relative piggybacks off of top.
      '#empty'      => $type == 'top' ? t('No one has been ranked yet.') : NULL,
      '#rows'       => $rows[$type],
      '#theme'      => 'table',
    );
  }
  $build['achievements']['leaderboard_pager'] = array(
    '#theme'  => 'pager',
  );

  return $build;
}

/**
 * Block callback; show the global leaderboard.
 *
 * @see achievement_leaderboard_totals()
 */
function achievements_leaderboard_block() {
  $header = array(array('data' => t('#'), 'sort' => NULL), t('Who'), t('Pts'));
  $achievers = $rows = array('top' => array(), 'relative' => array());

  $query = db_select('achievement_totals', 'at');
  $query->addTag('achievement_totals')->join('users', 'u', 'u.uid = at.uid');
  $query->fields('at', array('uid', 'points', 'unlocks', 'timestamp'))->fields('u', array('name'));
  $query->orderBy('at.points', 'DESC')->orderBy('at.timestamp'); // @bug DESC/ASC doesn't index.
  $query->range(0, variable_get('achievements_leaderboard_block_count_top', 5));
  $achievers['top'] = $query->execute()->fetchAllAssoc('uid');

  if (user_is_logged_in() && !isset($achievers['top'][$GLOBALS['user']->uid]) && variable_get('achievements_leaderboard_block_relative', 'nearby_ranks') != 'disabled') {
    $achievers['relative'] = (variable_get('achievements_leaderboard_block_relative', 'nearby_ranks') == 'nearby_ranks')
      ? achievements_totals_user(variable_get('achievements_leaderboard_block_relative_nearby_ranks', 1))
      : achievements_totals_user(); // you and me, togetheRrrr FoReEEvvvVeeEeeRRrRR.
  }

  $rank = 1; // innocent widdle variable!
  foreach (array('top', 'relative') as $type) {
    foreach ($achievers[$type] as $achiever) {
      $rows[$type][] = array(
        array(
          'data'  => isset($achiever->rank) ? $achiever->rank : $rank++,
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
    }
  }

  foreach (array('top', 'relative') as $type) {
    $build['achievements']['leaderboard'][$type] = array(
      '#attributes' => array('class' => array('achievement-leaderboard', 'achievement-leaderboard-' . $type)),
      '#header'     => $type == 'top' ? $header : NULL, // relative piggybacks off of top.
      '#empty'      => $type == 'top' ? t('No one has been ranked yet.') : NULL,
      '#rows'       => $rows[$type],
      '#theme'      => 'table',
    );
  }
  $build['achievements']['see-more'] = array(
    '#prefix'   => '<div class="achievement-see-more">',
    '#markup'   =>  l(t('View full leaderboard »'), 'achievements/leaderboard'),
    '#suffix'   => '</div>',
  );

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
  $achiever = array_pop(achievements_totals_user(0, $account->uid));

  $build['achievements']['stats'] = array(
    '#prefix' => '<div class="achievement-stats">',
    '#markup' => t('@name is ranked #@rank with @points points. @unlocks_count of @total_count achievements have been unlocked.', array(
      '@name'           => $account->name,
      '@rank'           => isset($achiever->rank) ? $achiever->rank : 0,
      '@points'         => isset($achiever->points) ? $achiever->points : 0,
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
          $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#weight'] = -$unlocks[$achievement_id]['timestamp'];
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
