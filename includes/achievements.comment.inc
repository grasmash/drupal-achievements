<?php
// $Id: bot.module,v 1.9.2.9.2.5 2008/04/26 01:32:30 morbus Exp $

/**
 * @file
 * Default comment-related achievements.
 */

/**
 * Implementation of hook_achievements_info().
 */
function achievements_comment_achievements_info() {
  return array(
    'comment-count-1' => array(
      'title'       => t('Posted your first comment'),
      'description' => t('You have successfully delurked. Welcome!'),
      'points'      => 5,
    ),

    'comment-count-5' => array(
      'title'       => t('Five feet high and rising'),
      'description' => t('You have posted five comments. Climbing, climbing...'),
      'points'      => 10,
    ),

    'comment-count-25' => array(
      'title'       => t('Quarter centenarian'),
      'description' => t('25 comments, great, but what about the children?'),
      'points'      => 25,
    ),

    'comment-count-100' => array(
      'title'       => t('Too many candles, not enough cake'),
      'description' => t("100 comments later and you're still here. Awesome!"),
      'points'      => 100,
    ),
  );
}

/**
 * Tweak the comment form to remove the anchor.
 *
 * This function is called from achievements.module's hook_form_alter().
 */
function achievements_comment_form_alter(&$form, $form_state, $form_id) {
  // for our comment achievements, new commenters will get sent to their
  // comment on a page, which is far below the actual "Achievement unlocked!"
  // status message. we have to hope they're going to comment (and unlock) it
  // here and remove the #comment-xyz anchor. note that we're testing here for
  // the /possibility/ of them earning the achievement - unless we move into
  // a #submit handler, we'll never know for sure if they get it. we'll only
  // stop the anchor if the user is close to getting an achievement.
  foreach (array(1, 5, 25, 100) as $count) { // this stops the normal usability from being negatively affected.
    if (!achievements_unlocked_already('comment-count-' . $count) && achievements_storage_get('comment_count') == $count - 1) {
      $form['#redirect'] = 'node/'. $form['nid']['#value'];
    }
  }
}
 
/**
 * Load all the achievement callbacks defined in this file.
 *
 * This function is called from achievements.module's hook_comment().
 */
function achievements_comment_init($a1, $op) {
  global $user;

  if ($op == 'insert' && $user->uid > 0) {
    achievements_comment_count($user->uid);
  }
}

/**
 * Count the number of comments the uid has made since we've been enabled.
 */
function achievements_comment_count($uid) {
  $counts = array(1, 5, 25, 100);

  foreach ($counts as $count) {
    // if even just one of our achievements is enabled, we need to count
    // comments. it'd be a bit lame, but someone could disable comment-1
    // but still want the rest of the comments (10, 100, 250, etc.).
    if (achievements_enabled('comment-count-' . $count)) {
      $counting_enabled = TRUE;
    }
  }

  if ($counting_enabled) {
    $current_count = achievements_storage_get('comment_count', $uid) + 1;
    achievements_storage_set('comment_count', $uid, $current_count);

    if (in_array($current_count, $counts)) {
      achievements_unlocked('comment-count-' . $current_count);
    }
  }
}
