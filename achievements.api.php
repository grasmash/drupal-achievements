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
 * module doesn't know how to unlock any of your achievements... instead, you
 * use Drupal's existing hooks, the achievement storage tables, and a few
 * helper functions to complete the workflow. See the remaining documentation
 * in this file for further code samples.
 *
 * There are many different kinds of achievements, and it's accurate enough to
 * say that if you can measure or respond to an action, it can be made into a
 * matching achievement. Be creative. Look at what others are doing. Have fun.
 * Your gamification efforts will fail or be un-fun if you don't have a gamer
 * helping you, if you make everything a mindless grind, or if you simply
 * copy achievements from another site or install.
 *
 * @return
 *   An array whose keys are internal achievement IDs (32 chars max) and whose
 *   values identify properties of the achievement. These properties are:
 *   - title: (required) The title of the achievement.
 *   - description: (required) A description of the achievement.
 *   - points: (required) How many points the user will earn when unlocked.
 *   - images: (optional) An array of (optional) keys 'locked', 'unlocked',
 *     and 'hidden' whose values image file paths. Achievements exist in one of
 *     three separate display states: unlocked (the user has it), locked (the
 *     user doesn't have it), and hidden (the user doesn't have it, and the
 *     achievement is a secret). Each state can have its own default image
 *     associated with it (which administrators can configure), or achievements
 *     can specify their own images for one, some, or all states.
 *   - storage: (optional) If you store statistics for your achievement, the
 *     core module assumes you've used the achievement ID for the storage
 *     location. If you haven't, specify the storage location here. This lets
 *     the core module know what to delete when an administrator manually
 *     removes an achievement unlock from a user. If your achievement
 *     tracks statistics that are NOT set with achievements_storage_get()
 *     or _set, you don't have to define the 'storage' key.
 *   - hidden: (optional) The achievement is a sekrit until it is unlocked.
 */
function hook_achievements_info() {
  $achievements = array(
    'comment-count-50' => array(
      'title'       => t('Posted 50 comments!'),
      'description' => t("We no longer think you're a spam bot. Maybe."),
      'storage'     => 'comment-count',
      'points'      => 50,
    ),
    'comment-count-100' => array(
      'title'       => t('Posted 100 comments!'),
      'description' => t('But what about the children?!'),
      'storage'     => 'comment-count',
      'points'      => 100,
      'images' => array(
        'unlocked'  => '/sites/default/files/example1.png',
        // 'hidden' and 'locked' will use the defaults.
      ),
    ),
    'node-mondays' => array(
      'title'       => t('Published some content on a Monday'),
      'description' => t("Go back to bed: it's still the weekend!"),
      'points'      => 5,
      'images' => array(
        'unlocked'  => '/sites/default/files/example1.png',
        'locked'    => '/sites/default/files/example2.png',
        'hidden'    => '/sites/default/files/example3.png',
        // all default images have been replaced.
      ),
    ),
  );

  return $achievements;
}

/**
 * Implements hook_comment_insert().
 */
function example_comment_insert($comment) {
  // Most achievements measure some kind of statistical data that must be
  // aggregated over time. To ease the storage of this data, the achievement
  // module ships with achievement_storage_get() and _set(), which allow you
  // to store custom data on a per-user basis. In most cases, the storage
  // location is the same as your achievement ID but in situations where you
  // have progressive achievements (1, 2, 50 comments etc.), it's better to
  // share a single place like we do below. If you don't use the achievement
  // ID for the storage location, you must specify the new location in the
  // 'storage' key of hook_achievements_info().
  //
  // Here we're grabbing the number of comments that the current commenter has
  // left in the past (which might be 0), adding 1 (for the current insert),
  // and then saving the count back to the database. The saved data is
  // serialized so can be as simple or as complex as you need it to be.
  $current_count = achievements_storage_get('comment-count', $comment->uid) + 1;
  achievements_storage_set('comment-count', $current_count, $comment->uid);

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
  // Secondly, the achievements_unlocked() function below automatically checks
  // if the user has unlocked the achievement already, and will not reward it
  // again if they have. This saves you a small bit of repetitive coding but
  // you're welcome to use achievements_unlocked_already() as needed.
  //
  // Knowing that we currently have 50 and 100 comment achievements, we simply
  // loop through each milestone and check if the current count value matches.
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
  // Sometimes, we don't need any storage at all.
  if (format_date(REQUEST_TIME, 'custom', 'D') == 'Mon') {
    achievements_unlocked('node-mondays', $node->uid);
  }
}
