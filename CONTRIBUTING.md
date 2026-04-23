# Contributing to Neva meta layer

## Neva meta layer branch rules

Default template: `<category>/<issue-id>/<description>`

Reference: https://dev.to/jps27cse/github-branching-name-best-practices-49ei

| Branch            | Role                                                | Example                                   |
|-------------------|-----------------------------------------------------|-------------------------------------------|
| feature/*         | For new or modification of features/functionalities | feature/NEVA-12345/user-authentication    |
| infra/*           | For CI/CD and other infra related works             | infra/NEVA-12345/add-pull-request-trigger |
| fix/*             | For fixing bugs                                     | fix/NEVA-12345/fix-current-cpu-assignment |
| test/*            | For writing or improving automated tests            | test/NEVA-12345/add-unit-tests            |
| doc/*             | For documentation updates                           | doc/NEVA-12345/add-pull-request-template  |

Branch naming rule:
- Every branch must fall into one of the categories shown in the table above.
- Use hyphens (-) to separate words for better readability.
  Don't use underbar (_) unless it's citation of var or another name.
- Avoid generic terms like update, changes, or stuff.

## Precondition

To clone and use the meta-neva repository, please refer to [README][readme].

Note: Further instructions assume working from meta-neva dir
which is clone of https://github.com/nevaoss/meta-neva.
Only contributors who was added to collaborators of nevaoss/meta-neva with
direct access can work with https://github.com/nevaoss/meta-neva directly
(the same way as Neva members). The other external contributors need to fork
https://github.com/nevaoss/meta-neva first, upload their change to that fork
and then suggest a PR to original nevaoss/meta-neva.

## Creating a change

First, create a new branch for your change in git. Here, we create a branch
called `doc/NEVA-11024/add-contributing-guidelines` (use name that follows
the guidelines described above), after first pulling the latest changes from
the webosose/843 branch:
```
$ git pull
$ git checkout -b doc/NEVA-11024/add-contributing-guidelines
```

Write and test your change.

- Conform to the [Yocto Project style guide][yp-styleguide].
- Patches should be a reasonable size to review. Review time often increases
  exponentially with patch size.

Commit your change locally in git:
```
$ git add <files>
$ git commit
```

After opening the text editor, you need to write a commit message following
this template:
```
[TagName] <summary line>

(can be omitted if summary line is informative enough)
Body <unformatted-text> 

Test Scenario:
<unformatted-text>

Issue: [Jira-ticket-number], [Jira-ticket-number]
```
Note: External contributors should first leave it as `Issue: none`
or `Issue: TBD` and then update when someone from Neva gives them
Jira issue number.

Example:
```
[sync_up][build] M148: Update webruntime version for Chromium v.148

Updated webruntime version from '147.0.7727.0' to '148.0.7778.0'.

Test Scenario:
1) Run webOS/OSE build
2) Check that the proper webruntime version is displayed in the build
log

Issue: NEVA-XXXXX
```
Note: Please, see which tags to use for a commit in the [Neva tag list](#neva-tag-list).

## Uploading a change for review

After writing a commit message for your changes, you need to push them
to the repository:
```
$ git push origin doc/NEVA-11024/add-contributing-guidelines
Enumerating objects: 4, done.
Counting objects: 100% (4/4), done.
Delta compression using up to 16 threads
Compressing objects: 100% (2/2), done.
Writing objects: 100% (3/3), 389 bytes | 2.00 KiB/s, done.
Total 3 (delta 1), reused 0 (delta 0), pack-reused 0
remote: Resolving deltas: 100% (1/1), completed with 1 local object.
remote:
remote: Create a pull request for 'doc/NEVA-11024/add-contributing-guidelines' on GitHub by visiting:
remote:      https://github.com/nevaoss/meta-neva/pull/new/doc/NEVA-11024/add-contributing-guidelines
remote:
To github.com:nevaoss/meta-neva.git
 * [new branch]                  doc/NEVA-11024/add-contributing-guidelines -> doc/NEVA-11024/add-contributing-guidelines
```

Next, you need to create a Pull Request either by clicking the link
that was automatically generated during the push, or manually on
the GitHub repository page.

Use the subject line from the commit message as the title
of the Pull Request, and fill in the description fields according
to the commit message.
But if there are multiple commits in one Pull Request, we should use
a general subject and description.
Example: https://github.com/nevaoss/meta-neva/pull/14

At the moment at least 2 approvals are required in order to merge Pull Request
into 'webosose/843'.
You can add reviewers on Conversation tab of the Pull Request.

## Pull Request and updating changes

Use `git rebase -i` command to change a stack of commits or
`git commit --amend` command to change one commit.

`--force` option must be added to `git push` command in order to update branch
in Pull Request:
```
$ git -c diff.ignoreSubmodules=all commit -a --amend
[doc/NEVA-11024/add-contributing-guidelines feb4baaca74e9] [TEST][DEMO] Add dummy-change.txt with numbers
 Date: Mon Mar 23 20:40:47 2026 +0300
 1 file changed, 5 insertions(+)
 create mode 100644 dummy-change.txt
$ git push origin doc/NEVA-11024/add-contributing-guidelines
To github.com:nevaoss/meta-neva.git
 ! [rejected]                    doc/NEVA-11024/add-contributing-guidelines -> doc/NEVA-11024/add-contributing-guidelines (non-fast-forward)
error: failed to push some refs to 'github.com:nevaoss/meta-neva.git'
hint: Updates were rejected because the tip of your current branch is behind
hint: its remote counterpart. Integrate the remote changes (e.g.
hint: 'git pull ...') before pushing again.
hint: See the 'Note about fast-forwards' in 'git push --help' for details.
$ git push --force origin doc/NEVA-11024/add-contributing-guidelines
Enumerating objects: 4, done.
Counting objects: 100% (4/4), done.
Delta compression using up to 16 threads
Compressing objects: 100% (2/2), done.
Writing objects: 100% (3/3), 398 bytes | 2.00 KiB/s, done.
Total 3 (delta 1), reused 0 (delta 0), pack-reused 0
remote: Resolving deltas: 100% (1/1), completed with 1 local object.
To github.com:nevaoss/meta-neva.git
 + c92264462937e...feb4baaca74e9 doc/NEVA-11024/add-contributing-guidelines -> doc/NEVA-11024/add-contributing-guidelines (forced update)
```

## Neva tag list

| Tag name        | Subtag name | Comment |
|-----------------|-------------|---------|
| ci              |             | CI (Continuous Integration) system configuration changes |
|                 | build       | Build rules, scripts, etc changes |
|                 | test        | Test scripts, etc changes |
| resources       |             | pak file changes |
| injection       |             | Changes in V8 injections mechanism and Web API (V8 injections) |
| pal             |             | Changes in Platform Abstraction Layer architecture and adding new PAL calls from Chromium |
| appruntime      |             | App run-time changes (aka former webOS WebView) |
| content         |             | Changes in Content API exported from Chromium/Blink (to be used from external components) |
| mixin           |             | Adding new mixins to Chromium class hierarchy (it's not about extending existing mixins with new functions) |
| g_ozone_wayland |             | Changes in Google wayland port for ozone |
| vkb             |             | Virtual KeyBoard |
| ime             |             | Input Method Editor |
| media           |             | Changes in Media API exported from Chromium and media backend |
|                 | uri         | Media player for URI based media playback |
|                 | mse         | Media player for Media Source Extention based media playback |
| cdm             |             | Changes for Content Decryption Module (Widevine, PlayReady, FairPlay) |
| webrtc          |             | WebRTC related patches (hardware accelerated preview, encode, decode) |
| wamdemo         |             | WAM demonstrator changes and PC emulator changes or PC implementation |
| browsershell    |             | Changes for Browser Shell |
| upstream        |             | The patch that is backported from upstream |
| upstreamable    |             | The patch that is to be upstreamed to Google |
| sync_up         |             | Upstream sync up |
| webos           |             | webOS specific code |
| memory          |             | Memory optimization |
| unittests       |             | Changes needed for Google unittests to compile and run |
| sandbox         |             | Changes for Chromium sandbox |
| neva_extensions |             | Changes for Neva's Chrome extensions support |
| wpt             |             | Changes for Web Platform Tests |
| gpu             |             | Changes related to GPU |
| webnn           |             | Changes related to WebNN |
| wam             |             | Changes related to WAM (Web Application Manager) |
| chromedriver    |             | Changes related to ChromeDriver |
| performance     |             | Performance optimization and benchmarks |
| docs            |             | Documentation changes (for example, .md files) |

[//]: # (the reference link section should be alphabetically sorted)
[readme]: https://github.com/nevaoss/meta-neva#bitbake-metas-and-recipes-via-mcf
[yp-styleguide]: https://docs.yoctoproject.org/contributor-guide/recipe-style-guide.html
