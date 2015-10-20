<?php

/**
 * @file
 * Contains Drupal\achievements\Tests\LeaderboardController.
 */

namespace Drupal\achievements\Tests;

use Drupal\simpletest\WebTestBase;

/**
 * Provides automated tests for the achievements module.
 *
 * @group achievements
 */
class LeaderboardControllerTest extends WebTestBase {

  /**
   * {@inheritdoc}
   */
  public function setUp() {
    parent::setUp();
  }

  /**
   * Tests achievements functionality.
   */
  public function testLeaderboardController() {
    // Check that the basic functions of module achievements.
    $this->assertEqual(TRUE, TRUE, 'Test Unit Generated via App Console.');
  }

}
