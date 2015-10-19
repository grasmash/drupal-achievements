<?php

/**
 * @file
 * Admin callbacks for the Achievements module.
 */

/**
 * Configure achievements.
 */
function achievements_settings() {

  return system_settings_form($form);
}

/**
 * Submit callback; clear achievement info cache.
 */
function achievements_clear_info_cache_submit() {
  achievements_load(NULL, FALSE, TRUE);
  drupal_set_message(t('Achievements information cache cleared.'));
}

/**
 * Submit callback; give or take away an achievement from a user.
 */
function achievements_manual_submit($form, &$form_state) {
  if (empty($form_state['values']['achievements_manual_user'])) { // ZOMG, it's #required, fool!
    drupal_set_message(t('A username or user ID is required to give or take achievements.'), 'error');
    return;
  }

  if (empty($form_state['values']['achievements_manual_achievement'])) { // HITCHCOCK STABBY STAB.
    drupal_set_message(t('An achievement ID is required to give or take achievements.'), 'error');
    return;
  }

  // well, we've got form values, so load 'em in and check for validity.
  $account = !is_numeric($form_state['values']['achievements_manual_user'])
    ? user_load_by_name($form_state['values']['achievements_manual_user'])
    : \Drupal::entityManager()->getStorage('user')->load($form_state['values']['achievements_manual_user']);
  $achievement = achievements_load($form_state['values']['achievements_manual_achievement']);

  if (!isset($account->uid) || $account->uid == 0) { // Wha? Who?
    drupal_set_message(t('%username is not a valid user.', // I TRIED TO HELP.
      array('%username' => $form_state['values']['achievements_manual_user'])), 'error');
    return;
  }

  if (!isset($achievement['title'])) { // -100 DKP! That's not even funny anymore.
    drupal_set_message(t('%achievement_id is not a valid achievement.', // M... many whelps?
      array('%achievement_id' => $form_state['values']['achievements_manual_achievement'])), 'error');
    return;
  }

  // if we've a valid user and achievement ID, give 'em the achievement, ya freeloaders.
  if ($account && $achievement && $form_state['clicked_button']['#achievement_action'] == 'give') {
    if (!$account->hasPermission('earn achievements')) { // KLAXON KLAXON CHEATAH.
      drupal_set_message(t('%username does not have permission to earn achievements.',
        array('%username' => $form_state['values']['achievements_manual_user'])), 'error');
      return;
    }

    achievements_unlocked($achievement['id'], $account->uid);
    drupal_set_message(t('%username has been given %achievement.', // yeah, yeah.
      array('%username' => format_username($account), '%achievement' => $achievement['title'])));
    return;
  }

  // if we're taking away an achievement, first check if they have it.
  if ($form_state['clicked_button']['#achievement_action'] == 'take') {
    if (!achievements_unlocked_already($achievement['id'], $account->uid)) {
      drupal_set_message(t('%username has not unlocked %achievement.', // palm to forehead.
        array('%username' => format_username($account), '%achievement' => $achievement['title'])), 'error');
      return;
    }

    achievements_locked($achievement['id'], $account->uid);
    drupal_set_message(t('%username has had %achievement taken away.', // nope, nope.
      array('%username' => format_username($account), '%achievement' => $achievement['title'])));
    return;
  }
}
