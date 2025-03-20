# eda-common

This repo contains the files for the additional exercise for lecture EDA ("Studienleistung zur Vorlesung EDA").

## Start

### Set git settings (example)

```
git config --global user.name "Stefanie Studi"
git config --global user.email stefanie.studi@stud.uni-hannover.de
```

### Clone working repo (example project eda000)

```
git clone git@gitlab.uni-hannover.de:lecture-eda/2024-WS/eda000
cd eda000
```

### Add additional remote eda-common

```
git remote add common git@gitlab.uni-hannover.de:lecture-eda/2024-WS/eda-common
git pull common main
```

## Start programming environment

```
code .
```

## Generate doxygen documentation in folder doc

```
doxygen implementation/Doxyfile
```

## Run all examples

```
./test.bash
```
