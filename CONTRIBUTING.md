# How to contribute efficiently

## Table of contents

- [Reporting bugs](#reporting-bugs)
- [Proposing features or improvements](#proposing-features-or-improvements)
- [Contributing pull requests](#contributing-pull-requests)
- [Contributing to Godot's translation](#contributing-to-godots-translation)
- [Communicating with developers](#communicating-with-developers)

**Please read the first section before reporting a bug!**

## Reporting bugs

Since Goblin is based on Godot 3.x branch any changes to Godot will eventually end up in Goblin.

With that in mind you can contribute directly to this project or indirectly though Godot project 3.x branch.

However, Goblin is not officially affiliated with Godot so you cannot ask Goblin specific questions on Godot channels because nobody can help you. You need to address your issues on Goblin channels. 

### Search first in the existing database

Issues are often reported several times by various users. It's good practice to
**search first in the [issue tracker](https://github.com/goblinengine/goblin/issues)
before reporting your issue**. If you don't find a relevant match or if you're
unsure, don't hesitate to **open a new issue**. 

- **[Report a bug](https://github.com/goblinengine/goblin/issues/new)**

## Proposing features or improvements

Since this is a one man project I cannot take feature proposals at this time.

You are welcome to submit PRs or suggest changes, however.

## Contributing pull requests

If you wish to contribute with code take a look at the official Godot 
[Engine development guide](https://docs.godotengine.org/en/latest/development/cpp/)
for an introduction to developing on Goblin/Godot.

### Document your changes

If your pull request adds methods, properties or signals that are exposed to
scripting APIs, you **must** update the class reference to document those.
This is to ensure the documentation coverage doesn't decrease as contributions
are merged.

[Update the documentation template](https://docs.godotengine.org/en/latest/community/contributing/updating_the_class_reference.html#updating-the-documentation-template)
using your compiled binary, then fill in the descriptions.
Follow the style guide described in the
[Docs writing guidelines](https://docs.godotengine.org/en/latest/community/contributing/docs_writing_guidelines.html).

If your pull request modifies parts of the code in a non-obvious way, make sure
to add comments in the code as well. 

### Be nice to the Git history

Try to make simple PRs that handle one specific issue or feature.

When updating upstream changes from Goblin use updating your fork with upstream changes, please use ``git pull --rebase``
to avoid creating "merge commits". Rebase will change the order of the commits to place your changes last and therefore making it cleaner. Merge commits unnecessarily pollute the git
history when coming from PRs.

Also try to make commits that bring the engine from one stable state to another
stable state, i.e. if your first commit has a bug that you fixed in the second
commit, try to merge them together before making your pull request (see ``git
rebase -i`` and relevant help about rebasing or amending commits on the
Internet).

This [Git style guide](https://github.com/agis-/git-style-guide) has some
good practices to have in mind.

See our [PR workflow](https://docs.godotengine.org/en/latest/community/contributing/pr_workflow.html)
documentation for tips on using Git, amending commits and rebasing branches.

### Format your commit messages with readability in mind

The way you format your commit messages is quite important to ensure that the
commit history and changelog will be easy to read and understand. A Git commit
message is formatted as a short title (first line) and an extended description
(everything after the first line and an empty separation line).

The short title is the most important part, as it is what will appear in the
`shortlog` changelog (one line per commit, so no description shown) or in the
GitHub interface unless you click the "expand" button. As the name says, try to
keep that first line under 72 characters. It should describe what the commit
does globally, while details would go in the description. Typically, if you
can't keep the title short because you have too much stuff to mention, it means
you should probably split your changes in several commits :)

Here's an example of a well-formatted commit message (note how the extended
description is also manually wrapped at 80 chars for readability):

```text
Prevent French fries carbonization by fixing heat regulation

When using the French fries frying module, Goblin would not regulate the heat
and thus bring the oil bath to supercritical liquid conditions, thus causing
unwanted side effects in the physics engine.

By fixing the regulation system via an added binding to the internal feature,
this commit now ensures that Goblin will not go past the ebullition temperature
of cooking oil under normal atmospheric conditions.

Fixes #1789
```

**Note:** When using the GitHub online editor or its drag-and-drop
feature, *please* edit the commit title to something meaningful. 

## Contributing to Godot's translation

You can contribute to Godot's translation from the [Hosted
Weblate](https://hosted.weblate.org/projects/godot-engine/godot), an open
source and web-based translation platform. Please refer to the [translation
readme](editor/translations/README.md) for more information.

You can also help translate [Godot's
documentation](https://hosted.weblate.org/projects/godot-engine/godot-docs/)
on Weblate.

## Communicating with developers

To communicate with developers (e.g. to discuss a feature you want to implement
or a bug you want to fix), the following channels can be used:

- [Bug tracker](https://github.com/goblinengine/goblin/issues): If there is an
  existing issue about a topic you want to discuss, just add a comment to it -
  many developers watch the repository and will get a notification. You can
  also create a new issue - please keep in mind to create issues only to
  discuss quite specific points about the development, and not general user
  feedback or support requests.

Thanks for your interest!
