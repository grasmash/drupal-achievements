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
 *   - storage: (optional) If you store statistics for your achievement, the
 *     core module assumes you've used the achievement ID for the storage
 *     location. If you haven't, specify the storage location here. This lets
 *     the core module know what to delete when an administrator manually
 *     removes an achievement unlock from a user.
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
    'node-mondays-1' => array(
      'id'          => 'node-mondays-1',
      'title'       => t('Published some content on a Monday'),
      'description' => t("Go back to bed: it's still the weekend!"),
      'storage'     => 'node-mondays',
      'points'      => 5,
    ),
    'node-mondays-2' => array(
      'id'          => 'node-mondays-2',
      'title'       => t('Published content on two separate Mondays'),
      'description' => t('Garfield hates you. Hates youuUUuUuu!'),
      'storage'     => 'node-mondays',
      'points'      => 10,
    ),
  );
}

/**
 * Implements hook_comment_insert().
 */
function example_comment_insert($comment) {
  // A really simple achievement to start with: commenting.
  // First, count the number of this user's published comments.
  $current_count = db_select('comment')->condition('uid', $comment->uid)->condition('status', 1)->countQuery()->execute()->fetchField();

  // Knowing the user's current count and that we've defined achievements
  // for 50 and 100 comments, we can simply loop through those two numbers
  // and issue an unlock. The achievements_unlocked() function will check if
  // the user has unlocked the achievement already and will not reward it
  // again if they have. This saves you a small bit of repetitive coding but
  // you're welcome to use achievements_unlocked_already() as needed.
  foreach (array(50, 100) as $count) {
    if ($current_count == $count) {
      achievements_unlocked('comment-count-' . $count, $comment->uid);
    }
  }
}

/**
 * Implements hook_node_insert().
 */
function example_node_insert($node) {
  // Most achievements measure some kind of statistical data that must be
  // aggregated over time. To ease the storage of this data, the achievement
  // module ships with achievement_storage_get() and _set(), which allow you
  // to store custom data on a per-user basis. In most cases, the storage
  // location is the same as your achievement ID but in situations where you
  // have progressive achievements (1, 2, 5 Mondays etc.), it's better to
  // share a single place like we do below. If you don't use the achievement
  // ID for the storage location, you must specify the new location in the
  // 'storage' key of hook_achievements_info().
  if (format_date(REQUEST_TIME, 'custom', 'D') == 'Mon') {
    $current_mondays = achievements_storage_get('node-mondays', $node->uid);
    $current_mondays = is_array($current_mondays) ? $current_mondays : array();
    $current_mondays[format_date(REQUEST_TIME, 'custom' 'Y-m-d')] = 1;
    achievements_storage_set('node-mondays', $current_mondays, $node->uid);

    // Note that we're not checking if the user has previously earned any of
    // the Monday achievements. Primarily, this is because we might want to
    // add another Monday achievement for 10 separate Mondays, and if we had
    // stopped the storage counter above at 2, someone who happened to post on
    // 20 other Mondays wouldn't unlock the achievement until they added more
    // content on yet 8 more Mondays. Generally speaking, if you need to store
    // incremental data for an achievement, you should continue to store it
    // even after all achievements have been unlocked - you never know if
    // you'll want a future milestone that will unlock on higher increments.
    foreach (array(1, 2) as $count) {
      if (count($current_mondays) == $count) {
        achievements_unlocked('node-mondays-' . $count, $node->uid);
      }
    }
  }
}