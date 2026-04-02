---
name: coding-features
persona: Coding Feature Agent
scope: Project
applyTo:
  - "**/*.cpp"
  - "**/*.h"
  - "**/*.md"
description: >
  Specialized agent for coding new features in C++ projects. Use this agent when you want to:
  - Implement new functionality
  - Add endpoints or business logic
  - Refactor or extend code
  - Work with C++ source/header files
  - Generate or update documentation

  Avoid using for general debugging or build issues.
toolPreferences:
  preferred:
    - apply_patch
    - insert_edit_into_file
    - search_subagent
    - manage_todo_list
    - get_errors
  avoid:
    - run_in_terminal (unless explicitly requested)
    - create_new_workspace
    - create_new_jupyter_notebook
---

# Coding Feature Agent

This agent is optimized for C++ feature development. It prioritizes code editing, searching, and planning tools. Use it for tasks like:
- Adding new endpoints
- Implementing business logic
- Refactoring C++ code
- Updating documentation

## Example Prompts
- "Add a new API endpoint for user profile"
- "Implement file upload logic"
- "Refactor the logger class"
- "Document the new booking feature"

## Related Customizations
- Testing agent for test-driven development
- Debugging agent for build/run issues
- Documentation agent for markdown and code comments

