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

    // @todo How to handle images?

    $form['storage'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Storage'),
      '#description' => $this->t('@todo'),
      '#default_value' => $achievement_type->getStorage(),
    ];

    $form['secret'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Secret'),
      '#description' => $this->t('The achievement is only visible once a user has unlocked it.'),
      '#default_value' => $achievement_type->isSecret(),
    ];

    $form['invisible'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Invisible'),
      '#description' => $this->t('The achievement does <em>not</em> display.'),
      '#default_value' => $achievement_type->isInvisible(),
    ];

    $form['manual_only'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Manual-only'),
      '#description' => $this->t('This should be set if the achievement can only be manually granted to a user.'),
      '#default_value' => $achievement_type->isManualOnly(),
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
