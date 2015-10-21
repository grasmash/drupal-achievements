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

    $achievement_type = $this->entity;
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $achievement_type->label(),
      '#description' => $this->t("Label for the Achievement type."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $achievement_type->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\achievements\Entity\AchievementType::load',
      ),
      '#disabled' => !$achievement_type->isNew(),
    );

    /* You will need additional form elements for your custom properties. */

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
