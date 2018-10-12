<?php

namespace Drupal\achievements\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class AchievementEntityForm.
 */
class AchievementEntityForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    /** @var \Drupal\achievements\Entity\AchievementEntity $achievement_entity */
    $achievement_entity = $this->entity;
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $achievement_entity->label(),
      '#description' => $this->t("Label for the Achievement entity."),
      '#required' => TRUE,
    ];

    $form['id'] = [
      '#type' => 'machine_name',
      '#default_value' => $achievement_entity->id(),
      '#machine_name' => [
        'exists' => '\Drupal\achievements\Entity\AchievementEntity::load',
      ],
      '#disabled' => !$achievement_entity->isNew(),
    ];

    $form['description'] = [
      '#type' => 'textarea',
      '#title' => $this->t('Description'),
      '#required' => TRUE,
      '#default_value' => $achievement_entity->getDescription(),
      '#description' => $this->t('The description of the achievement.'),
    ];

    $form['secret'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Secret'),
      '#description' => $this->t('The achievement is only visible once a user has unlocked it.'),
      '#default_value' => $achievement_entity->isSecret(),
    ];

    $form['invisible'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Invisible'),
      '#description' => $this->t('The achievement does <em>not</em> display.'),
      '#default_value' => $achievement_entity->isInvisible(),
    ];

    $form['points'] = [
      '#type' => 'number',
      '#title' => $this->t('Points'),
      '#description' => $this->t('The point value of the achievement.'),
      '#default_value' => $achievement_entity->getPoints(),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $achievement_entity = $this->entity;
    $status = $achievement_entity->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Achievement entity.', [
          '%label' => $achievement_entity->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Achievement entity.', [
          '%label' => $achievement_entity->label(),
        ]));
    }
    $form_state->setRedirectUrl($achievement_entity->toUrl('collection'));
  }

}
