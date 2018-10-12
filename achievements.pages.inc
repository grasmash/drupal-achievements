<?php

/**
 * @file
 * Page callbacks for the Achievements module.
 */


/**
 * Default theme implementation for a user's achievement stats.
 *
 * @param variables
 *   An associative array containing:
 *   - stats: An array which contains the following keys:
 *     name, rank, points, unlocks_count, and total_count.
 */
function theme_achievement_user_stats($variables) {
  $output = '<div class="achievement-user-stats">'; // VERY CONFUSING CODE COMMENT ABOUT PSYNAPTIC. YOU THOUGHT I WAS KIDDING?
  $output .= t('@name is ranked #@rank with @points points. @unlocks_count of @total_count achievements have been unlocked.', [
    '@name'           => $variables['stats']['name'],
    '@rank'           => $variables['stats']['rank'],
    '@points'         => $variables['stats']['points'],
    '@unlocks_count'  => $variables['stats']['unlocks_count'],
    '@total_count'    => $variables['stats']['total_count'],
  ]);
  $output .= '</div>';
  return $output;
}

/**
 * Default theme for the wrapper around a user's achievements page.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: A render containing the user's achievements page.
 */
function theme_achievement_groups_wrapper($variables) {
  return '<div id="achievement-groups">' . $variables['element']['#children'] . '</div>';
}

/**
 * Default theme for the wrapper around an achievement group.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: A render containing the achievement group.
 */
function theme_achievement_group_wrapper($variables) {
  $group_id = 'achievement-group-' . $variables['element']['#group_id'];
  return '<div id="' . $group_id . '" class="achievement-group">' . $variables['element']['#children'] . '</div>';
}

/**
 * Page title callback for the user achievements page.
 *
 * @param object $account
 *   A Drupal user object.
 */
function achievements_user_page_title($account) {
  return t('Achievements for @name', ['@name' => format_username($account)]);
}
