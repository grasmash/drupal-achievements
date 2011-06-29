<?php
// $Id: bot.module,v 1.9.2.9.2.5 2008/04/26 01:32:30 morbus Exp $

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
  global $user; // what's mah score if i'm not in the list?!
  $ranks = achievements_totals_info();

  if ($block) {
    $rows = array(); for ($i = 1; $i <= 5; $i++) {
      if (!$ranks['by_rank'][$i]) { continue; }
      $rows[] = array( // we don't have 5 poeple?! ARGH!
        $i, theme('username', (object)$ranks['by_rank'][$i]),
        l($ranks['by_rank'][$i]['points'], 'user/'. $ranks['by_rank'][$i]['uid'] .'/achievements'),
      );
    }

    // if the current user sucks, let 'em know <g>
    if ($ranks['by_user'][$user->uid]['rank'] > 5) {
      $rows[] = array( // not in the top 5? poor poor user. add 'em cos we love 'em.
        $ranks['by_user'][$user->uid]['rank'], theme('username', (object)$ranks['by_user'][$user->uid]),
        l($ranks['by_user'][$user->uid]['points'], 'user/'. $user->uid .'/achievements'),
      );
    }

    if (count($rows) == 0) { // did you just install the module?
      $rows[] = array(array('data' => t('No one has made it yet.'), 'colspan' => 5));
    }

    $output  = theme('table', array(), $rows, array('class' => 'achievements-leaderboard-totals-block'));
    $output .= '<div style="text-align:center;">'. l(t('View the top 50 leaders'), 'achievements/leaderboard') .'</div>';
  }
  else {
    drupal_set_title(t('Achievement leaderboard')); // page title is different from menu title.
    $headers = array(array('data' => t('#'), 'field' => NULL), t('User'), t('Points'), t('Last scored'));
    // we have to NULL out field for the first column so that it doesn't earn the "active" class in CSS.

    $rows = array(); for ($i = 1; $i <= 50; $i++) {
      if (!$ranks['by_rank'][$i]) { continue; } // deleted?
      $rows[] = array( // we don't have 50 people? FOR SHAME!
        $i, theme('username', (object)$ranks['by_rank'][$i]),
        l($ranks['by_rank'][$i]['points'], 'user/'. $ranks['by_rank'][$i]['uid'] .'/achievements'),
        format_date($ranks['by_rank'][$i]['timestamp'], 'small'), // I'"""m IInNN the mOOoOdooD for lOoovoe.
      );
    }

    // if the current user sucks, let 'em know :(
    if ($ranks['by_user'][$user->uid]['rank'] > 50) {
      $rows[] = array( // not in the top 50? poor poor user. add 'em cos they're not working hard enough.
        $ranks['by_user'][$user->uid]['rank'], theme('username', (object)$ranks['by_user'][$user->uid]),
        l($ranks['by_user'][$user->uid]['points'], 'user/'. $user->uid .'/achievements'), //me clears throat. ahem:
        format_date($ranks['by_user'][$user->uid]['timestamp'], 'small'), // I'"""m IInNN the mOOoOdooD stiIILlLllLL.
      );
    }

    if (count($rows) == 0) { // just install the module? i suppose there's nothing here yet.
      $rows[] = array(array('data' => t('No one has made it on the leaderboard yet.'), 'colspan' => 5));
    } // note to self: make an achievement for installing the module. chuckle at your silliness.

    $output  = theme('table', $headers, $rows, array('class' => 'achievements-leaderboard-totals'));
  }

  return $output;
}

/**
 * Menu callback; display a single achievement page with leaderboards.
 */
function achievements_leaderboard_for($achievement) {
  drupal_add_css(drupal_get_path('module', 'achievements') .'/achievements.css');
  drupal_set_title(t('Achievement: @title', array('@title' => $achievement['title'])));
  $output = NULL; // the dead are rising from their graves!

  $headers = array(array('data' => t('Points'), 'field' => NULL), t('Achievement'));
  $rows[] = theme('achievement', $achievement); // we want this one slightly bigger, so -detail it.
  $output .= theme('table', $headers, $rows, array('class' => 'achievement-leaderboard-detail'));
  $output .= achievements_unlocked_already($achievement['id']);

  // @todo should we cache this? watch it under heavy usage.
  $results = array(); // saves a lot of duplicate code by looping.
  $results['first']  = db_query("SELECT au.uid, u.name, au.rank, au.timestamp FROM {achievement_unlocks} au LEFT JOIN {users} u ON (au.uid = u.uid) WHERE achievement_id = '%s' AND rank <= 5 ORDER BY rank", $achievement['id']);
  $results['recent'] = db_query_range("SELECT au.uid, u.name, au.rank, au.timestamp FROM {achievement_unlocks} au LEFT JOIN {users} u ON (au.uid = u.uid) WHERE achievement_id = '%s' ORDER BY timestamp DESC", $achievement['id'], 0, 5);

  $output .= '<div class="achievement-unlocked">';
  foreach (array('first', 'recent') as $type) { // loop through both results.
    $rows = array(); while ($result = db_fetch_object($results[$type])) {
      $rows[] = array(
        $result->rank,
        theme('username', $result), // we need a class on our points just to center it. rowr, lame.
        l(achievements_totals_info($result->uid), 'user/'. $result->uid .'/achievements'),
        format_date($result->timestamp, 'small')
      );
    }

    if (count($rows) < 1) {
      $rows[] = array(array('data' => t('No one has unlocked this yet. Keep trying!'), 'colspan' => 4));
    } // see, the "Keep trying!" is inspirational. wouldn't you want to be first? WOUDLN"T YOUW?E!

    $output .= '<div class="achievement-unlocked-'. $type .'">';
    $output .=  '<h3>'. t('!type achievement unlocks', array('!type' => drupal_ucfirst($type))) .'</h3>';
    $output .=  theme('table', array(), $rows) .'</div>';
  } $output .= '</div>';

  return $output;
}

/**
 * Menu callback; display all achievements for the passed user.
 *
 * @param $user
 *   The user object this request applies against.
 */
function achievements_user_page($user) {
  drupal_add_css(drupal_get_path('module', 'achievements') .'/achievements.css');
  drupal_set_title(t('Achievements for @name', array('@name' => $user->name)));
  $output = NULL; // together, we will rule the O of the desired I!

  $achievements = achievements_load();
  $results = db_query('SELECT achievement_id, rank, timestamp FROM {achievement_unlocks} WHERE uid = %d ORDER BY timestamp DESC', $user->uid);
  $headers = array(array('data' => t('Points'), 'field' => NULL), t('Achievement'), t('Date unlocked'));
  while ($result = db_fetch_object($results)) { // get over it. those were my panties. not yours.
    $rows[] = theme('achievement', $achievements[$result->achievement_id], $result->timestamp);
  }

  // spit out stats about this user, including goose eggs if the user doesn't have much of anything.
  $output .= '<div class="achievement-current-rank">'. t('@name is currently ranked #!rank with @points points.', array('@name' => $user->name, '!rank' => achievements_totals_info($user->uid, 'rank'), '@points' => achievements_totals_info($user->uid))) . '</div>';
  $output .= '<div class="achievement-total-remaining">' . t('!unlocked_count of !total_count achievements have been unlocked.', array('@name' => $user->name, '!unlocked_count' => count($rows), '!total_count' => count(achievements_load_enabled()))) . '</div>';

  if (count($rows) == 0) { // you really want to have 0 achievements? i mean, you get achievements for *everything*! do *something*!
    $rows[] = array(array('data' => t("@name has not yet unlocked any achievements.", array('@name' => $user->name)), 'colspan' => 3));
  }

  $output .= theme('table', $headers, $rows);
  return $output;
}

/**
 * Configures achievements.
 */
function achievements_settings() {
  $form = array(); // helLOO, nurse!
  $achievements = achievements_load();
  $form['achievements'] = array(
    '#collapsed'        => FALSE,
    '#collapsible'      => TRUE,
    '#description'      => t('All achievements are enabled by default. Disable an achievement by unchecking its box. In most cases, if you disable an achievement you also disable its data storage. For example, disabling the various comment counting achievements will stop comment counting entirely; if a user comments 24 times with the achievement disabled, and then you enable it, he would not earn an unlock for his 25th comment since, to the internal bean counter, it\'d only be considered his first. Finally, disabling an achievement merely stops future users from unlocking it - it does not remove existing unlocks or decrease existing point values.'),
    '#title'            => t('Achievements list'),
    '#tree'             => TRUE,
    '#type'             => 'fieldset',
  );
  foreach ($achievements as $achievement) { // loop through each achievement and set the magick.
    $form['achievements'][$achievement['id']] = array(
      '#achievement'   => $achievement,
      '#default_value' => achievements_enabled($achievement['id']),
      '#type'          => 'checkbox',
    );
  }

  $form = system_settings_form($form);
  $form['#theme'] = 'achievements_settings_form';
  $form['#submit'][] = 'achievements_settings_form_submit';
  return $form;
}

/**
 * Submit callback for achievements_settings().
 */
function achievements_settings_form_submit($form, &$form_state) {
  // if the form has been submitted, expire the achievements data cache.
  achievements_load(NULL, TRUE);
}

/**
 * Theme the administration page.
 */
function theme_achievements_settings_form($form) {
  drupal_add_css(drupal_get_path('module', 'achievements') .'/achievements.css');
  $achievements = achievements_load(); // easier than looping through the form.
  $header = array(array('data' => t('Enabled'), 'class' => 'checkbox'), t('Points'), t('Description'));

  foreach ($achievements as $achievement) { 
    $row = array(
      array('data' => drupal_render($form['achievements'][$achievement['id']]), 'class' => 'checkbox'),
      array('data' => '+' . $achievement['points'], 'class' => 'achievement-points'),
      '<div class="achievement-title">' . l($achievement['title'], 'achievements/leaderboard/' . $achievement['id']) . '</div>'.
        '<div class="achievement-description">' . $achievement['description'] . '</div>',
    );

    $rows[] = $row;
  }

  $form['achievements']['#value'] = theme('table', $header, $rows, array('class' => 'achievements-admin-list'));
  $output .= drupal_render($form);
  return $output;
}
