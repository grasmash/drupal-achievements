<?php

namespace Drupal\achievements\Controller;

use Drupal\Component\Utility\Xss;
use Drupal\Core\Controller\ControllerBase;
use Drupal\user\UserInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Database\Driver\mysql\Connection;
use Drupal\Core\Config\ConfigFactoryInterface;

/**
 * Class AchievementsController.
 */
class AchievementsController extends ControllerBase {

  /**
   * Drupal\Core\Database\Driver\mysql\Connection definition.
   *
   * @var \Drupal\Core\Database\Driver\mysql\Connection
   */
  protected $database;
  /**
   * Drupal\Core\Config\ConfigFactoryInterface definition.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * Constructs a new AchievementsController object.
   */
  public function __construct(Connection $database, ConfigFactoryInterface $config_factory) {
    $this->database = $database;
    $this->configFactory = $config_factory;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('database'),
      $container->get('config.factory')
    );
  }

  /**
   * User achievements page title.
   *
   * @param \Drupal\user\UserInterface $user
   *   The user account.
   *
   * @return array
   *   The page title.
   */
  public function userAchievementsTitle(UserInterface $user) {
    return $user ? ['#markup' => "Achievements for " . $user->getUsername(), '#allowed_tags' => Xss::getHtmlTagList()] : '';
  }

  /**
   * User achievements.
   *
   * @param \Drupal\user\UserInterface $user
   *   The user account.
   *
   * @return array
   *   Render array of the user's achievements.
   */
  public function userAchievements(UserInterface $user) {
    $unlocks = achievements_unlocked_already(NULL, $user->id());
    $achievers = achievements_totals_user($user->id());
    $achiever = reset($achievers);
    $achievements = achievements_load_all();

    $build['stats'] = [
      '#theme'  => 'achievement_user_stats',
      '#stats'  => [
        'name'          => $user->getDisplayName(),
        'rank'          => isset($achiever->rank) ? $achiever->rank : 0,
        'points'        => isset($achiever->points) ? $achiever->points : 0,
        'unlocks_count' => count($unlocks),
        'total_count'   => count($achievements),
      ],
    ];

    $build['#theme_wrappers'] = ['container'];
    $build['#attributes'] = ['class' => ['achievements']];
    $build['#attached'] = [
      'library' => [
        'achievements/achievements',
      ],
    ];

    foreach ($achievements as $achievement_id => $achievement) {
      if (!empty($achievement->isInvisible()) && !isset($unlocks[$achievement_id])) {
        // Invisibles only display if this $account has unlocked them.
        continue;
      }

      $locked_weight = 0;
      // If it's not an invisible achievement, we've got to show something.
      // $build out what and where.
      $build[$achievement_id]['#achievement_entity'] = $achievement;
      $build[$achievement_id]['#theme'] = 'achievement';

      $achievements_unlocked_move_to_top = \Drupal::config('achievements.settings')
        ->get('achievements_unlocked_move_to_top');

      if (isset($unlocks[$achievement_id])) {
        $build[$achievement_id]['#unlock'] = $unlocks[$achievement_id];
        if ($achievements_unlocked_move_to_top) {
          $build[$achievement_id]['#weight'] = -$unlocks[$achievement_id]['timestamp'];
          // By setting the negative weight to the timestamp,
          // the latest unlocks are always shown at the top.
        }
      }
      elseif (!isset($unlocks[$achievement_id]) && $achievements_unlocked_move_to_top) {
        // if we're forcing unlocks to the top, locked achievements have to be forced to the bottom.
        $build[$achievement_id]['#weight'] = $locked_weight++;
      }
    }

    return $build;
  }

}
