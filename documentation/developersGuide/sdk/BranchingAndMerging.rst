=============================================
Creating and Maintaining Development Branches
=============================================

**bring up to date for BZR!**

When work on a separate branch?
-------------------------------

- You intend to develop a certain, cohesive feature in one of the WNS modules?
- It will take you a number of days to develop it?
- During this period you want to do interim commits whithout having to bother about leaving the mainline WNS in non-working state?
- You are tired of merging other people's changes with your changes before being able to commit them?


Then a private development branch may just be the right thing for you
to use. 

Bazaar and its Separation of Commit, Push and Pull Operation
------------------------------------------------------------

Bazaar is a distributed version control system. The most important difference to CVS or
to subversion is that it separates the procedure of making commits and the procedure
of publication (called *push* in Bazaar) and retrieval (*pull*) patches to/from  a remote location.
A commit is always made locally to your copy of the branch (if you created a branch). If you want
to publish your modifications you need to @em push it somewhere.

If you want to merge with your parent branch you need to push your changes back to the
parent branch, i.e. the one you originally branched from. If you want to make it available
to other users, but you do not want to merge simply push your branches to another location.
If there would be conflicts, then Bazaar will inform you and suggest that you do a @em merge.


Creating a branch from the WNS Mainline
---------------------------------------

By default the modules included in the openWNS SDK are already private branches. Each
of the module is a full copy of its including all its history information. In fact,
you do not need to create a private branch - you already have one.


Obtaining Mainline Patches
--------------------------
@todo to be written

Merging back
------------
@todo to be written
