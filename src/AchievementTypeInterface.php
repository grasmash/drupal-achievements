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

  /**
   * Gets the storage.
   *
   * @return string
   */
  public function getStorage();

  /**
   * Determine if the achievement is secret.
   *
   * @return bool
   *   Returns TRUE if the achievement is secret.
   */
  public function isSecret();

  /**
   * Determine if the achievement is invisible.
   *
   * @return bool
   *   Returns TRUE if the achievement is invisible.
   */
  public function isInvisible();

  /**
   * Determine if an achievement can only be granted manually.
   *
   * @return bool
   *   Returns TRUE if the achievement can only be granted manually.
   */
  public function isManualOnly();

}
