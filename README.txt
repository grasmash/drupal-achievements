
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Creating achievements


INTRODUCTION
------------

Current Maintainer: Morbus Iff <morbus@disobey.com>

The Achievements module is intended to emulate Xbox 360 achievements, a
system whereby gamers are awarded points for the completion of game-specific
challenges. For a Drupal site, this could mean commenting a certain number of
times, starting a forum topic, visiting the site every day of the week, or
any number of other possibilities.

There are a few conceptual differences between Achievements and User Points:

 * Achievement points are based on milestones, not activity. Whereas, in User
   Points, a user may get 5 points every time he posts a node, a matching
   achievement would instead reward 100 points for posting 10 nodes (or 50,
   etc.) If there's no achievement for posting 20 nodes, the user receives
   no further points.

 * Since achievements are "milestones", each one has its own leaderboard
   that lists when a user has met (or "unlocked") the goal, and their
   matching rank. A site-wide leaderboard ranks users by points they've
   achieved, but also by timestamp -- if two users share the same point
   total, the person who got there first gets the higher rank.

 * Achievements can be hidden so that a user doesn't know how to unlock
   it until he stumbles upon the discovery himself (either by meeting the
   milestone, asking another user, etc.).

 * Achievements are not retroactively applied - you either get them after
   the Achievements module has been enabled, or you don't get them at all.
   Likewise, achievements can not be deleted (but that may change).

 * Unlike the points of User Points, a user never loses his achievement
   points for, say, an e-commerce purchase or other redeemable item.
   Fundamentally, a user can never go down in score. If you need to "shame"
   your users, you can create 0 point achievements like "Had 5 comments
   deleted", "Was blocked", etc. Shame achievements are not fun.


CREATING ACHIEVEMENTS
---------------------

I've made two... ... "odd" but entirely deliberate design decisions:

 1) No achievements are shipped by default. Why? Earning the same achievement
    over and over again at dozens of Drupal sites is mind-numbingly not-fun.
    If you're going to offer achievements, at least _try_ to be creative and
    make them unique to your site.

 2) Creating achievements requires custom code. Why? Achievements that can
    be automated in a user interface tend to be mind-numbingly not-fun and
    grindish ("when user creates $n comments, do..."). Quality achievements
    require custom logic tailored to your site.

I do believe that achievement grinds have their place and I do want to be
rewarded for posting 1000 comments or 250 nodes. I just don't want to see
default implementations of those achievements on every site that uses this
module. It's lazy. It's not-fun. It reflects poorly on my code if I promote
cookie-cutter gamification on Drupal sites everywhere.

Yes, the above is a barrier to using the module.

And, yes, I think it will improve the overall fun-ness of its usage.

To learn how to create achievements, see achievements.api.php.
