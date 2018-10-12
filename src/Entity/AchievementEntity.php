<?php

namespace Drupal\achievements\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Defines the Achievement entity entity.
 *
 * @ConfigEntityType(
 *   id = "achievement_entity",
 *   label = @Translation("Achievement entity"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\achievements\AchievementEntityListBuilder",
 *     "form" = {
 *       "add" = "Drupal\achievements\Form\AchievementEntityForm",
 *       "edit" = "Drupal\achievements\Form\AchievementEntityForm",
 *       "delete" = "Drupal\achievements\Form\AchievementEntityDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\achievements\AchievementEntityHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "achievement_entity",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/achievement_entity/{achievement_entity}",
 *     "add-form" = "/admin/structure/achievement_entity/add",
 *     "edit-form" = "/admin/structure/achievement_entity/{achievement_entity}/edit",
 *     "delete-form" = "/admin/structure/achievement_entity/{achievement_entity}/delete",
 *     "collection" = "/admin/structure/achievement_entity"
 *   }
 * )
 */
class AchievementEntity extends ConfigEntityBase implements AchievementEntityInterface {

  /**
   * The Achievement entity ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Achievement entity label.
   *
   * @var string
   */
  protected $label;


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
   * The number of points an achievement is worth.
   *
   * @var int
   */
  protected $points;

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
  public function getPoints() {
    return $this->points;
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


}