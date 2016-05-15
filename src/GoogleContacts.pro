TEMPLATE = subdirs

SUBDIRS = Data MainApp MainAppConsole Tests

Data.subdir = Data

MainApp.subdir = MainApp
MainApp.depends = Data
MainApp.depends = Tests

MainAppConsole.subdir = MainAppConsole
MainAppConsole.depends = Data

Tests.subdir = Tests
Tests.depends = Data
