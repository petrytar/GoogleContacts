TEMPLATE = subdirs

SUBDIRS = Data MainApp MainAppConsole

Data.subdir = Data

MainApp.subdir = MainApp
MainApp.depends = Data

MainAppConsole.subdir = MainAppConsole
MainAppConsole.depends = Data
