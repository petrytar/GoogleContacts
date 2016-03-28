TEMPLATE = subdirs

SUBDIRS = Data MainApp

Data.subdir = Data

MainApp.subdir = MainApp
MainApp.depends = Data
