<?php

/**
 * @file
 * Contains Drupal\achievements\Controller\LeaderboardController.
 */

namespace Drupal\achievements\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Class LeaderboardController.
 *
 * @package Drupal\achievements\Controller
 */
class LeaderboardController extends ControllerBase {
  /**
   * Leaderboard.
   *
   * @return string
   *   Return Hello string.
   */
  public function leaderboard() {
    return [
        '#type' => 'markup',
        '#markup' => $this->t('Implement method: leaderboard')
    ];
  }

}
