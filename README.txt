
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Creating achievements


INTRODUCTION
------------

Current Maintainer: Morbus Iff <morbus@disobey.com>

The Achievements module offers the ability to create achievements and
badges similar to systems seen on the Xbox 360, Playstation 3, Foursquare,
Gowalla, GetGlue, and more. For a Drupal site, this could mean commenting a
certain number of times, starting a forum topic, visiting the site every day
of the week, or anything else that can be tracked and coded.

Current features and design:

 * Achievement points are based on milestones, not continuous activity.
   Instead of getting 5 points every time a user posts a node, an equivalent
   milestone might instead reward 20 points for posting 10 nodes. If there's
   no achievement for posting 50 nodes, the user receives no further points.

 * Since achievements are milestones, each one has its own leaderboard
   that lists when a user has met (or "unlocked") the goal and their
   matching rank. A site-wide leaderboard ranks users by points they've
   achieved, but also by timestamp -- if two users share the same point
   total, the person who got there first gets the higher rank.

 * Relative leaderboards are supported and allow the user to see where they
   are in relation to nearby ranks or the top achievers. Leaderboards also
   show the latest achievement earned, allowing users to discover new
   milestones they might want to strive for.

 * Achievements can be hidden so that a user doesn't know how to unlock
   it until she stumbles upon the discovery herself (meeting the milestone,
   asking a user, etc.).

 * Achievements can have images (or "badges") in one of three different
   states: unlocked, locked, or hidden. Default images can be used for all
   achievements (and some are provided with the module), or you can override
   them on a per-achievement basis.

 * Achievements can be grouped into categories and tabbed with jQuery UI.

 * An adminterface allows you to manually grant or remove achievements. If
   the user is offline at the time, any unlocked achievements will display
   the next time the user visits the site.

 * Your code decides whether achievements are retroactively applied or not.


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
