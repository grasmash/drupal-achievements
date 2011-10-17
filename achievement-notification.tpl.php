<?php

/**
 * @file
 * Default theme implementation for an achievement notification popup.
 *
 * Available variables:
 * - $achievement: The achievement being displayed, as an array.
 * - $unlock: An array of unlocked 'rank' and 'timestamp', if applicable.
 * - $unlocked_date: A formatted date of the user's unlock timestamp.
 * - $unlocked_rank: A formatted string of the user's unlock ranking.
 * - $image: The achievement's image (default or otherwise), linked.
 * - $image_raw: The raw path to the context-aware achievement image.
 * - $achievement_url: Direct URL of the current achievement.
 * - $classes: String of classes for this achievement.
 */
?>
<div class="<?php print $classes; ?>">
  <div class="achievement-image"><?php print $image; ?></div>

  <div class="achievement-points-box">
    <div class="achievement-points"><?php print $achievement['points']; ?></div>
    <div class="achievement-unlocked-stats">
      <div class="achievement-unlocked-rank"><?php print $unlocked_rank; ?></div>
    </div>
  </div>

  <div class="achievement-body">
    <div class="achievement-header">Achievement Unlocked</div>
    <div class="achievement-title"><a href="<?php print $achievement_url; ?>" tabindex="-1"><?php print $achievement['title']; ?></a></div>
  </div>
</div>
