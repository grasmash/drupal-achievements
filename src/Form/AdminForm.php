<?php

/**
 * @file
 * Contains Drupal\achievements\Form\AdminForm.
 */

namespace Drupal\achievements\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Config\ConfigFactoryInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Config\ConfigFactory;

/**
 * Class AdminForm.
 *
 * @package Drupal\achievements\Form
 */
class AdminForm extends ConfigFormBase {

  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory')
    );
  }


  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['achievements.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'achievements_admin_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('achievements.settings');

    $form['achievements_rankings'] = [
     '#description' => $this->t('These settings affect the <a href=":url">global leaderboard</a>. Enabling the "relative leaderboard" will show the current user\'s position and, optionally, a number of ranks before and after that position. For example, if the current user is ranked 12th and you configure 3 nearby ranks, the relative leaderboard will show ranks 9 through 15.', [':url' => $this->url('achievements.leaderboard')]),
      '#title' => $this->t('Leaderboard ranks'),
      '#type' => 'fieldset',
    ];

    $form['achievements_rankings']['leaderboard_count_per_page'] = [
      '#type' => 'select',
      '#title' => $this->t('Number of top ranks per page'),
      '#default_value' => $config->get('leaderboard_count_per_page'),
      '#options' => array_combine([5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100], [5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100]),
    ];
    $form['achievements_rankings']['leaderboard_relative'] = [
      '#type' => 'radios',
      '#title' => $this->t('Relative leaderboard display'),
      '#default_value' => $config->get('achievements_leaderboard_relative'),
      '#options' => [
        'disabled' => $this->t("Don't show the relative leaderboard"),
        'user_only' => $this->t('Show only the current user'),
        'nearby_ranks' => $this->t('Show the current user and nearby ranks'),
      ],
    ];
    $form['achievements_rankings']['leaderboard_relative_nearby_ranks'] = [
      '#type' => 'select',
      '#title' => $this->t('Number of nearby ranks to display'),
      '#default_value' => $config->get('achievements_leaderboard_relative_nearby_ranks'),
      '#options' => array_combine(range(1, 10), range(1, 10)),
    ];

    $form['achievements_config'] = array(
      '#title' => $this->t('Additional configuration'),
      '#type' => 'fieldset',
    );
    $module_path = drupal_get_path('module', 'achievements');
    foreach (array('unlocked', 'locked', 'secret') as $image_type) {
      // @FIXME
// // @FIXME
// // The correct configuration object could not be determined. You'll need to
// // rewrite this call manually.
// $form['achievements_config']['achievements_image_' . $image_type] = array(
//       '#default_value'      => variable_get('achievements_image_' . $image_type, $module_path . '/images/default-' . $image_type . '-70.jpg'),
//       '#title'              => t('Default @image_type picture', array('@image_type' => $image_type)),
//       '#type'               => 'textfield',
//     );

    }
    $form['achievements_config']['unlocked_move_to_top'] = array(
      '#type' => 'checkbox',
      '#title' => $this->t("Move the user's unlocked achievements to the top of their achievement page."),
      '#default_value' => $config->get('unlocked_move_to_top'), // default to gamer-style.
    );

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $settings = [
      'leaderboard_count_per_page',
      'leaderboard_relative',
      'leaderboard_relative_nearby_ranks',
      'unlocked_move_to_top',
    ];
    foreach ($settings as $name) {
      $this->config('achievements.settings')
        ->set($name, $form_state->getValue($name));
    }

    $this->config('achievements.settings')->save();
    parent::submitForm($form, $form_state);
  }

}
