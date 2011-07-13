<?php

/**
 * @file
 * Default theme implementation of a single achievement's display.
 *
 * Available variables:
 * - $achievement: The achievement being displayed, as an array.
 * - $unlock: Optional; an array of unlocked 'rank' and 'timestamp'.
 */
  $achievement_class = isset($unlock) ? 'unlocked' : 'locked';
  if (isset($achievement['hidden']) && !achievements_unlocked_already($achievement['id'])) {
    $achievement['points']      = t('??');
    $achievement['title']       = t('Hidden achievement');
    $achievement['description'] = t('Continue playing to unlock this hidden achievement.');
    $achievement_class          = 'hidden';
  }

  $image_path = isset($achievement['images'][$achievement_class]) ? $achievement['images'][$achievement_class] // do whatcha want, eh? it's yer theme, bub.
    : variable_get('achievements_image_' . $achievement_class, drupal_get_path('module', 'achievements') . '/images/default-' . $achievement_class . '-70.jpg');
?>

<div class="achievement achievement-<?php print $achievement_class; ?>">
  <div class="achievement-image"><?php print theme('image', array('path' => $image_path)); ?></div>

  <div class="achievement-points-box">
    <div class="achievement-points"><?php print $achievement['points']; ?></div>
    <div class="achievement-unlocked-stats">
      <div class="achievement-unlocked-timestamp"><?php print isset($unlock['timestamp']) ? format_date($unlock['timestamp'], 'custom', 'Y/m/d') : ''; ?></div>
      <div class="achievement-unlocked-rank"><?php print isset($unlock['rank']) ? t('Rank #@rank', array('@rank' => $unlock['rank'])) : ''; ?></div>
    </div>
  </div>

  <div class="achievement-body">
    <div class="achievement-title"><?php print l($achievement['title'], 'achievements/leaderboard/' . $achievement['id']); ?></div>
    <div class="achievement-description"><?php print $achievement['description']; ?></div>
  </div>
</div>
