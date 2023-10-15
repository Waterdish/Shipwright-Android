# Preface
LUS accepts any and all contributions. You can interact with the project via PRs, issues, email (kenixwhisperwind@gmail.com), or [Discord](https://discord.gg/RQvdvavB).

# Code of Conduct
Please review and abide by our [code of conduct](https://github.com/Kenix3/libultraship/blob/main/CODE_OF_CONDUCT.md).

# Building
Please see the [readme](https://github.com/Kenix3/libultraship/blob/main/README.md) for building instructions.

# Pull Requests
## Procedures
Our CI system will automatically check your PR to ensure that it fits [formatting guidelines](https://github.com/Kenix3/libultraship/blob/main/.clang-format), [linter guidelines](https://github.com/Kenix3/libultraship/blob/main/.clang-tidy), and that the code builds. The submitter of a PR is required to ensure their PR passes all CI checks. The submitter of the PR is encouraged to address all PR review comments in a timely manner to ensure a timely merge of the PR.
### Troubleshooting CI Errors
#### tidy-format-validation
If the tidy-format-validation check fails, then you need to run clang-format. Below is a command that can be used on Linux. Most modern IDEs have a clang-format plugin that can be used. The [.clang-format file can be found here](https://github.com/Kenix3/libultraship/blob/main/.clang-format)
##### Running clang-format
```
find src include -name "*.cpp" -o -name "*.h" | sed 's| |\\ |g' | xargs clang-format-12 -i
```
