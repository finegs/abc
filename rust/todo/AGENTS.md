# Project & Global Agent Instructions (`AGENTS.md`)

## Code Editing & File Reuse Guidelines

- **Maximize Reuse of Existing Files**: Re-use existing structs, helper functions, schemas, and signatures as much as possible.
- **Minimal Block Modifications**: Do not rewrite full functions or output entire files. Provide inline line-range modifications only.
- **Use Targeted Editing Tools**: Use fine-grained line-range tools (`replace_file_content` / `multi_replace_file_content`) specifying precise start and end lines. Avoid overwriting complete files.
