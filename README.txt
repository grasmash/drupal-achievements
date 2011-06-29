// $Id: INSTALL.txt,v 1.61.2.2 2008/02/07 20:46:56 goba Exp $

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * API and Examples


INTRODUCTION
------------

The Achievements module is intended to emulate Xbox 360 achievements, a
system whereby gamers are awarded points for the completion of game-specific
challenges. For a Drupal site, this could mean commenting a certain number of
times, starting a forum topic, visiting the site every day of the week, or
any number of other possibilities.

There are a few conceptual differences between Achievements and User Points:

 * Unlike the points of User Points, a user never loses his achievement
   points for, say, an e-commerce purchase or other redeemable item.

 * Unlike the points of User Points, achievement points are based on
   milestones, not necessarily activity. Whereas, in User Points, a user
   may get 5 points every time he posts a node, a matching achievement
   would instead reward 100 points for posting 10 nodes (or 50, etc.)

 * Since achievements are "milestones", each one has its own leaderboard
   that lists when a user has met (or "unlocked") the goal, and their
   matching rank. A site-wide leaderboard ranks users by points they've
   achieved, but also by timestamp -- if two users share the same point
   total, the person who got there first gets the higher rank.

 * Achievements can be hidden so that a user doesn't know how to unlock
   it until he stumbles upon the discovery himself (either by meeting the
   milestone, asking another user, etc.).

 * Unlike User Points, the only achievement data stored in the database
   is the achievement's internal ID and when the user unlocked it. This
   allows you to change the achievement details (spelling, name, etc.)
   without having to issue database updates.

 * Achievements are defined via an API hook, like Drupal's menus and forms,
   and can be enabled or disabled by an admin. This allows modules to ship
   with dozens of achievements, but gives the admin the ability to shut off
   specific milestones that may not apply to his user base.

 * Achievements are not retroactively applied - you either get them after
   the Achievements module has been enabled, or you don't get them at all.
   Likewise, achievements can not be deleted (but that may change).

 * Fundamentally, a user can never go down in score. If you need to "shame"
   your users, you can create 0 point achievements like "Had 5 comments
   deleted", "Was blocked", etc. The Achievements module does not ship
   with any of these shame achievements.


INSTALLATION
------------

1. Copy the files to your sites/SITENAME/modules directory.
   Or, alternatively, to your sites/all/modules directory.

2. Enable the Achievements module at admin/build/modules.

3. All achievements start out enabled, but you can disable
   an individual milestone at admin/settings/achievements.

4. To test an achievement, comment on an existing node.


API AND EXAMPLES
----------------
