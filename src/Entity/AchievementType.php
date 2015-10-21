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
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->description;
  }

}
