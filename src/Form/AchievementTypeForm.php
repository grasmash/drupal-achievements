<?php

/**
 * @file
 * Contains Drupal\achievements\Form\AchievementTypeForm.
 */

namespace Drupal\achievements\Form;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class AchievementTypeForm.
 *
 * @package Drupal\achievements\Form
 */
class AchievementTypeForm extends EntityForm {
  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    /** @var \Drupal\achievements\AchievementTypeInterface $achievement_type */
    $achievement_type = $this->entity;
    $form['title'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Title'),
      '#maxlength' => 255,
      '#default_value' => $achievement_type->label(),
      '#required' => TRUE,
    ];

    $form['id'] = [
      '#type' => 'machine_name',
      '#default_value' => $achievement_type->id(),
      '#machine_name' => [
        'exists' => '\Drupal\achievements\Entity\AchievementType::load',
        'source' => ['title'],
      ],
      '#disabled' => !$achievement_type->isNew(),
    ];

    $form['description'] = [
      '#type' => 'textarea',
      '#title' => $this->t('Description'),
      '#required' => TRUE,
      '#default_value' => $achievement_type->getDescription(),
      '#description' => $this->t('The description of the achievement.'),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $achievement_type = $this->entity;
    $status = $achievement_type->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Achievement type.', [
          '%label' => $achievement_type->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Achievement type.', [
          '%label' => $achievement_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($achievement_type->urlInfo('collection'));
  }

}
