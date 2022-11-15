import java.io.*;
import java.lang.reflect.InvocationTargetException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public class Run {

    /**
     * Function ran when injection into JVM was successful.
     *
     * @param path Original path of program that injected this code
     */
    public static void run(String path) {
        System.out.println("Loading...");
        try {
            loadJar(path);
            System.out.println("JAR loaded.");
        } catch (Exception exception) {
            exception.printStackTrace(); // JAR load failed.
        }
    }


    /**
     * Load a JAR file into the environment
     * @param dir Directory to find run.jar
     */
    private static void loadJar(String dir) throws IOException, ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        System.out.println("Fetching JAR file (from " + dir + ")...");
        File file = new File(dir, "run.jar");
        JarFile jar = new JarFile(file);
        System.out.println("Fetching run.txt...");
        JarEntry entry = jar.getJarEntry("run.txt");
        if (entry == null) return;
        System.out.println("Reading run.txt...");
        BufferedReader reader = new BufferedReader(new InputStreamReader(jar.getInputStream(entry)));
        String main = reader.readLine();

        jar.close();
        System.out.println("Loading main class...");
        try (URLClassLoader classLoader = URLClassLoader.newInstance(new URL[]{file.toURI().toURL()}, ClassLoader.getSystemClassLoader())) { // Class loader that loads from JAR
            Class<?> clazz = Class.forName(main, false, classLoader); // Find given main class from JAR
            System.out.println("Running main method...");
            clazz.getMethod("main").invoke(null); // Run static void function called "main"
        }
    }
}