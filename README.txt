
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Creating achievements


INTRODUCTION
------------

Current Maintainer: Morbus Iff <morbus@disobey.com>

The Achievements module emulates standard gameplay achievements, a system
whereby gamers are awarded points for the completion of game-specific
challenges. For a Drupal site, this could mean commenting a certain number
of times, starting a forum topic, visiting the site every day of the week,
or anything else that can be tracked and coded.

Current features and design:

 * Achievement points are based on milestones, not continuous activity.
   Instead of getting 5 points every time a user posts a node, an equivalent
   milestone might instead reward 20 points for posting 10 nodes. If there's
   no achievement for posting 20 nodes, the user receives no further points.

 * Since achievements are milestones, each one has its own leaderboard
   that lists when a user has met (or "unlocked") the goal, and their
   matching rank. A site-wide leaderboard ranks users by points they've
   achieved, but also by timestamp -- if two users share the same point
   total, the person who got there first gets the higher rank.

 * Achievements can be hidden so that a user doesn't know how to unlock
   it until he stumbles upon the discovery himself (either by meeting the
   milestone, asking another user, etc.).

 * Achievements are not retroactively applied - you either get them after
   the Achievements module has been enabled, or you don't get them at all.
   Likewise, achievements can not be deleted (but they can be taken away).


CREATING ACHIEVEMENTS
---------------------

I've made two entirely deliberate design decisions:

 1) No achievements are shipped by default. Earning the same achievement
    over and over again at dozens of Drupal sites is mind-numbingly not-fun.
    If you're going to offer achievements, at least _try_ to be creative and
    make them unique to your site.

 2) Creating achievements requires custom code. Achievements that can be
    automated in a user interface tend to be mind-numbingly not-fun and
    grindish ("when user creates $n comments, $n posts", etc.). Quality
    achievements require custom logic tailored to your site.

I _do_ believe that achievement grinds have their place and I _do_ want to
be rewarded for posting 1000 comments or 250 nodes. I just don't want to see
default implementations on every site that uses this module. It's lazy. It's
not-fun. It reflects poorly on my code and achievement whoredom if I promote
cookie-cutter gamification on Drupal sites everywhere.

To learn how to create achievements, see achievements.api.php.
