# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is a Qt learning repository containing multiple independent projects organized by topic. Each subdirectory is a standalone Qt project demonstrating different Qt features and components.

## Build System

This repository uses **qmake** as the primary build system (`.pro` files). Some projects also have Visual Studio solution files (`.sln`).

### Building a Project

Each project is built independently:

```bash
# Using qmake
cd <project-directory>
qmake
make

# Or with Qt Creator - open the .pro file directly
```

## Project Structure

- **Core/** - Qt core components (QThreadPool, C++11 threading, graphics, callbacks)
- **UI/** - QtWidgets UI controls (QTableView, QGraphicsView, QQFriendList, Toast notifications)
- **QSS/** - Qt Style Sheets (QSS) for widget styling
- **QtSQL/** - Qt database operations (SQLite)
- **Tools/** - Utility libraries (QtEmail SMTP library, ToolsMainWindow framework)
- **Animation/** - Qt animation examples
- **PyQt/** - Python PyQt examples
- **QtExcel/** - Excel integration via COM
- **Project/** - Complete example projects (AppTranslation, QRCodeDemo, QmlUseCppModel, etc.)

## Key Libraries

- **QtEmail** (Tools/QtEmail/) - SMTP email library with MIME support
- **ToolsMainWindow** (Tools/ToolsMainWindow/) - Reusable main window framework with dialogs
