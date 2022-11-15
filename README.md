# java-injector
Inject any JAR file into a running JVM.

# How to Use
**NOTE: This program will most likely trigger your anti-virus**
1. Download the latest [release](https://github.com/birthdates/java-injector/releases/) or build it yourself (if you build it yourself, create a libs folder and move `runner/Run.java` into it)
2. Move all files into one folder
3. Compile your JAR and move it into the same folder but rename it to `run.jar`
4. Launch your Java application (`javaw.exe` application)
5. Run `injector.exe` and the JAR file should be injected into the JVM (if not, steps were not followed correctly)

# How to Debug
You will need to build the program yourself. Head into `shared.h` and uncomment the line that looks like `#define VERBOSE`. After this, rebuild the project.

# Common Problems
1. `My Java code does not execute!` - There is likely an error within your code, run the program in verbose mode.
2. `Nothing injects (not even console)` - There is likely another `javaw.exe` running.
3. `ClassNotFoundException` -  The JVM you injected into does not have this class!
4. `My prints aren't showing even in verbose mode!` - Prints from other threads will not show up in verbose mode.