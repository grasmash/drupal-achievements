<?php

/**
 * @file
 * Contains Drupal\achievements\AchievementTypeInterface.
 */

namespace Drupal\achievements;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining Achievement type entities.
 */
interface AchievementTypeInterface extends ConfigEntityInterface {

  /**
   * Gets the description.
   *
   * @return string
   *   The description of this achievment type.
   */
  public function getDescription();

}
