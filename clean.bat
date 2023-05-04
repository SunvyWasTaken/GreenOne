@echo off

rmdir /s /q Saved

rmdir /s /q Intermediate

rmdir /s /q DerivedDataCache

rmdir /s /q .idea

rmdir /s /q .vs

del /q .vsconfig

del /q *.sln


