<?php

/**
 * @file
 * Default theme implementation of a single achievement's display.
 *
 * Available variables:
 * - $achievement: The achievement being displayed, as an array.
 * - $unlock: Optional; an array of unlocked 'rank' and 'timestamp'.
 */
  // if the achievement is hidden, a user without it sees no information.
  if (isset($achievement['hidden']) && !achievements_unlocked_already($achievement['id'])) {
    $achievement['points']      = t('?');
    $achievement['title']       = t('Hidden achievement');
    $achievement['description'] = t('Continue playing to unlock this hidden achievement.');
  }
?>

<div class="achievement <?php print isset($unlock) ? 'achievement-unlocked' : 'achievement-locked'; ?>">
  <div class="achievement-body">
    <div class="achievement-title"><?php print l($achievement['title'], 'achievements/leaderboard/' . $achievement['id']); ?></div>
    <div class="achievement-description"><?php print $achievement['description']; ?></div>
  </div>

  <div class="achievement-points-box">
    <div class="achievement-points"><?php print $achievement['points']; ?></div>
    <div class="achievement-unlocked">
      <div class="achievement-unlocked-timestamp"><?php print isset($unlock['timestamp']) ? format_date($unlock['timestamp'], 'custom', 'Y/m/d') : ''; ?></div>
      <div class="achievement-unlocked-rank"><?php print isset($unlock['rank']) ? t('Rank #@rank', array('@rank' => $unlock['rank'])) : ''; ?></div>
    </div>
  </div>
</div>
