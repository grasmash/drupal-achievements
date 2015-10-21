<?php

/**
 * @file
 * Contains Drupal\achievements\Entity\AchievementType.
 */

namespace Drupal\achievements\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\achievements\AchievementTypeInterface;

/**
 * Defines the Achievement type entity.
 *
 * @ConfigEntityType(
 *   id = "achievement_type",
 *   label = @Translation("Achievement type"),
 *   handlers = {
 *     "list_builder" = "Drupal\achievements\AchievementTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\achievements\Form\AchievementTypeForm",
 *       "edit" = "Drupal\achievements\Form\AchievementTypeForm",
 *       "delete" = "Drupal\achievements\Form\AchievementTypeDeleteForm"
 *     }
 *   },
 *   config_prefix = "achievement_type",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "title",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/achievement_type/{achievement_type}",
 *     "edit-form" = "/admin/structure/achievement_type/{achievement_type}/edit",
 *     "delete-form" = "/admin/structure/achievement_type/{achievement_type}/delete",
 *     "collection" = "/admin/structure/visibility_group"
 *   },
 *   config_export = {
 *     "id",
 *     "title",
 *     "description",
 *     "storage",
 *     "secret",
 *     "invisible",
 *     "manual_only",
 *     "points"
 *   }
 * )
 */
class AchievementType extends ConfigEntityBase implements AchievementTypeInterface {
  /**
   * The Achievement type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Achievement type label.
   *
   * @var string
   */
  protected $title;

  /**
   * The achievement type description.
   *
   * @var string
   */
  protected $description;

  /**
   * The achievement storage.
   *
   * @var string
   */
  protected $storage;

  /**
   * Flag denoting the achievement is invisible.
   *
   * @var bool
   */
  protected $invisible;

  /**
   * Flag denoting the achievement is secret.
   *
   * @var bool
   */
  protected $secret;

  /**
   * Flag denoting the achievment is manual-only.
   *
   * @var bool
   */
  protected $manual_only;

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->description;
  }

  /**
   * {@inheritdoc}
   */
  public function getStorage() {
    return $this->storage;
  }

  /**
   * {@inheritdoc}
   */
  public function isSecret() {
    return $this->secret;
  }

  /**
   * {@inheritdoc}
   */
  public function isInvisible() {
    return $this->invisible;
  }

  /**
   * {@inheritdoc}
   */
  public function isManualOnly() {
    return $this->manual_only;
  }

}
