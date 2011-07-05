<?php

/**
 * @file
 * Hooks provided by the Achievements module and how to implement them.
 */

/**
 * Define an achievement.
 *
 * This hook describes your achievements to the base module so that it can
 * create the pages and caches necessary for site-wide display. The base
 * module doesn't know how to unlock any of your defined achievements...
 * instead, you use Drupal's existing hooks, the achievement storage tables,
 * and a few helper functions to complete the workflow. See the remaining
 * documentation in this file for further code samples.
 *
 * @return
 *   An array whose keys are internal achievement IDs and whose values
 *   identify properties of the achievement. These properties are:
 *   - id: (required) Internal ID of the achievement (32 character max.)
 *   - title: (required) The title of the achievement.
 *   - description: (required) A description of the achievement.
 *   - points: (required) How many points the user will earn when unlocked.
 *   - hidden: (optional) The achievement is a sekrit until it is unlocked.
 */
function hook_achievements_info() {
  return array(
    'comment-count-50' => array(
      'id'          => 'comment-count-50',
      'title'       => t('Posted 50 comments!'),
      'description' => t("We no longer think you're a spam bot. Maybe."),
      'points'      => 50,
    ),
    'comment-count-100' => array(
      'id'          => 'comment-count-100',
      'title'       => t('Posted 100 comments!'),
      'description' => t('But what about the children?!'),
      'points'      => 100,
    ),
    'node-mondays-2' => array(
      'id'          => 'node-mondays-2',
      'title'       => t('Created a node on two separate Mondays.'),
      'description' => t('Garfield hates you. Hates youuUUuUuu!'),
      'points'      => 10,
    ),
  );
}

/**
 * Implements hook_comment_insert().
 */
function example_comment_insert($comment) {
  // Most achievements measure some kind of statistical data that must be
  // aggregated over time. To ease the storage of this data, the achievement
  // module ships with achievement_storage_get() and _set(), which allow you
  // to store custom data on a per-user basis. In most cases, the storage
  // key is the same as your achievement ID but in situations where you have
  // progressive achievements (10, 20, 50, 100 comments etc.), it's better
  // to use a single key shared amongst them.

  // Here we're grabbing the number of comments that the current commenter has
  // left in the past (which might be 0), adding 1 (for the current insert),
  // and then saving the count back to the database. The saved data is
  // serialized so can be as simple or as complex as you need it.
  $current_count = achievements_storage_get('comment-count') + 1;
  achievements_storage_set('comment-count', $current_count);

  // Note that we're not checking if the user has previously earned any of the
  // commenting achievements yet. There are two reasons: first, we might want
  // to add another commenting achievement for, say, 250 comments, and if we
  // had stopped the storage counter above at 100, someone who currently has
  // 300 comments wouldn't unlock the achievement until they added another 150
  // nuggets of wisdom to the site. Generally speaking, if you need to store
  // incremental data for an achievement, you should continue to store it even
  // after the achievement has been unlocked - you never know if you'll want
  // to add a future milestone that will unlock on higher increments.
  //
  // Secondly, the achievements_unlocked() function below will automatically
  // check if the user has unlocked the achievement already, and will not
  // unlock it again if they have. This saves you a small bit of repetitive
  // coding but you're welcome to use achievements_unlocked_already() should
  // the need every arise.
  //
  // Knowing that we currently have 50 and 100 comment achievements, we simply
  // loop through each milestone and check if the current count value matches.
  foreach (array(50, 100) as $count) {
    if ($current_count == $count) {
      achievements_unlocked('comment-count-' . $count);
    }
  }
}

/**
 * Implements hook_node_insert().
 */
function example_node_insert($node) {
  // This is very similar to the comment counting achievement, but shows
  // storage of an array and a slightly different unlocking check. Yawn.
  if (format_date(REQUEST_TIME, 'custom', 'D') == 'Mon') {
    $current_mondays = achievements_storage_get('node-mondays');
    $current_mondays = is_array($current_mondays) ? $current_mondays : array();
    $current_mondays[format_date(REQUEST_TIME, 'custom' 'Y-m-d')] = 1;
    achievements_storage_set('node-mondays', $current_mondays);

    if (count($current_mondays) == 2) {
      achievements_unlocked('node-mondays-2');
    }
  }
}