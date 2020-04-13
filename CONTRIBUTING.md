# How to contribute to secREtary

# Guidelines

If you want to contribute to the project, feel free to send a pull request.

- Commit messages should follow the following template:
```
dir/path: one-line description
<empty line>
Extended multi-line description that includes
the problem you are solving and how it is solved.
```
`dir/path` is a relative path to the main dir this commit changes

Please pay attention to punctuation. In particular:
- `one-line description` does *not* start with a Capital letter.
- there is *no dot* at the end of `one-line description`.
- `Extended multi-line description` is full English sentenses with Capital letters and dots.

Also note:
- If you commit fixes an issue, please include `Fixes #NNN` line into commit message
(where `NNN` is issue number). This will auto-close the issue. If you need to mention
an issue without closing it, add `Update #NNN`.
- Rebase your pull request onto the master branch before submitting.
